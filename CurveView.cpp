//
//  CurveView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#include "CurveView.h"
#include "PointView.h"
#include <QGraphicsPathItem>
#include <QDebug>
#include <QPen>
#include <assert.h>

CurveView::CurveView(std::shared_ptr<CurveModel> model, QGraphicsItem* parent)
:   TransformationNode(parent),
    m_model(model),
    m_snapGridRect(QRectF()),
    m_snapToGrid(false),
    m_highlightCurve(false)
{
    m_spline = new Spline;
    m_curveView = new QGraphicsPathItem(this);
    
    connect(m_model.get(), &CurveModel::valueRangeChanged, this, &CurveView::valueRangeChanged);

    m_highlightCurve = m_model->isSelected();
    connect(m_model.get(), &CurveModel::selectedChanged, this, &CurveView::highlightCurve);
    
    connect(m_model.get(), &CurveModel::pointAdded, this, &CurveView::pointAdded);
    connect(m_model.get(), &CurveModel::pointUpdated, this, &CurveView::pointUpdated);
    connect(m_model.get(), &CurveModel::pointRemoved, this, &CurveView::pointRemoved);
    
    for (auto pid : m_model->pointIds())
    	pointAdded(pid);

    updateTransformation();
}

CurveView::~CurveView()
{
}

void CurveView::setSnapGrid(QRectF gridRect)
{
    if (m_snapGridRect != gridRect)
    {
        m_snapGridRect = gridRect;
        if (m_snapToGrid)
            emit snapGridChanged(m_snapGridRect);
    }
}

void CurveView::setSnapToGrid(bool snapToGrid)
{
    if (m_snapToGrid != snapToGrid)
    {
        m_snapToGrid = snapToGrid;
        emit snapGridChanged(m_snapToGrid ? m_snapGridRect : QRectF());
    }
}

void CurveView::duplicateSelectedPoints()
{
    qDebug() << "CurveView::duplicateSelectedPoints";

    QSet<PointId> toBeDuplicated;

    for (auto &point : m_pointViews)
        if (point->isSelected())
            toBeDuplicated.insert(point->pointId());

    for (auto pid : toBeDuplicated)
    {
        const Point point = m_model->point(pid);

        const PointId nextPointId = m_model->nextPointId(pid);
        if (nextPointId.isValid())
        {
            // Add point to between this and the next point matching the curve value at that point
            const Point nextPoint = m_model->point(nextPointId);
            float insertTime = (point.time() + nextPoint.time()) / 2.0f;
            float insertValue = m_spline->value_at(insertTime);
            m_model->addPoint(insertTime, insertValue);
        }
        else
        {
            // Add point to +1 second from the original point with the same value(s)
            m_model->addPoint(point.time() + 1.0, point.value());
        }
    }
}

void CurveView::removeSelectedPoints()
{
    QSet<PointId> toBeRemoved;

    for (auto &point : m_pointViews)
        if (point->isSelected())
            toBeRemoved.insert(point->pointId());

    for (auto pid : toBeRemoved)
        m_model->removePoint(pid);
}

void CurveView::highlightCurve(bool highlight)
{
    if (m_highlightCurve != highlight)
    {
        m_highlightCurve = highlight;
        updateCurves();
    }
}


void CurveView::pointAdded(PointId id)
{
    qDebug() << "CurveView::pointAdded" << id;
    assert(m_model);
    
    assert(id.isValid());
    assert(!findPointView(id));

    PointView* pointView = new PointView(m_model->point(id), this);
    m_pointViews.insert(id, pointView);
    connect(pointView, &PointView::pointPositionChanged, m_model.get(), &CurveModel::updatePoint);
    connect(pointView, &PointView::pointSelectedChanged, m_model.get(), &CurveModel::pointSelectedChanged);
    connect(this, &CurveView::snapGridChanged, pointView, &PointView::setSnapGrid);
    pointView->setSnapGrid(m_snapGridRect);

    bool addOk = addToSpline(id);
    assert(addOk);
    
    updateCurves();
}

void CurveView::pointUpdated(PointId id)
{
    qDebug() << "CurveView::pointUpdated" << id;
    assert(id.isValid());

    PointView* pointView = findPointView(id);
    assert(pointView);
    pointView->setPoint(m_model->point(id));

    bool removeOk = removeFromSpline(id);
    assert(removeOk);
    
    bool addOk = addToSpline(id);
    assert(addOk);
    
    updateCurves();
}

void CurveView::pointRemoved(PointId id)
{
    qDebug() << "CurveView::pointRemoved" << id;
    
    PointView* pointView = findPointView(id);
    assert(pointView);
    delete pointView;

    int numberOfRemoved = m_pointViews.remove(id);
    assert(numberOfRemoved == 1);

    bool removeOk = removeFromSpline(id);
    assert(removeOk);
    
  	updateCurves();
}

void CurveView::valueRangeChanged(RangeF /*valueRange*/)
{
    updateTransformation();
}

PointView* CurveView::findPointView(PointId id) const
{
    return m_pointViews.value(id, nullptr);
}

void CurveView::updateCurves()
{
    static const float ESPILON = 0.1;

    QPen curvePen(Qt::blue);
    curvePen.setCosmetic(true);
    if (m_highlightCurve)
    {
        // Highlight curve by boosting width and darkening color
        curvePen.setWidth(curvePen.width() + 1);
        curvePen.setColor(curvePen.color().darker(100));
    }

    auto cur = m_spline->data().begin();
    if (cur == m_spline->data().end())
    {
        // No spline data
        m_curveView->setPath(QPainterPath());
        return;
    }

    constexpr int STEPS_PER_INTERVAL = 20;

    // Start by moving tot the first point
    QPainterPath path;
    path.moveTo(QPointF(cur->time(), cur->value()));

    auto next = cur + 1;
    while (next != m_spline->data().end())
    {
        const float startTime = cur->time();
        const float endTime = next->time();

        const float time_diff = endTime - startTime;

        if (time_diff > ESPILON)
        {
            // If consequtive points have meaningful time difference between them approximate the curve with STEPS_PER_INTERVAL straight lines
            const float step = time_diff / STEPS_PER_INTERVAL;

            // Start from startTime + step as the first point is handled either
            // before the loop (moveTo) or by the previous interval. Only loop
            // until the last point before endTime as the last line is draw outside the
            // loop.
            float time = startTime;
            for (int i = 0; i < STEPS_PER_INTERVAL - 1; ++i)
            {
                time += step;
                path.lineTo(QPointF(time, m_spline->value_at(time)));
            }
        }

        // End the curve section to the next point
        path.lineTo(QPointF(endTime, next->value()));
        ++cur;
        ++next;
    }
        
    m_curveView->setPen(curvePen);
    m_curveView->setPath(path);
}

bool CurveView::addToSpline(PointId id)
{
    if (!id.isValid())
        return false;

    const Point point = m_model->point(id);
    const CurveModel::KbParams params = m_model->params(id);

    const pt::math::kochanek_bartels_parameters kb_params(params.tension(), params.bias(), params.continuity());
    pt::math::kb_data_set<float>::point p(id, point.time(), point.value().toFloat(), kb_params);
    auto point_iter = m_spline->data().add(p);
    if (point_iter == m_spline->data().end())
        return false;
    
    return true;
}

bool CurveView::removeFromSpline(PointId id)
{
    if (!id.isValid())
        return false;
    
    auto point_iter = findSplinePoint(id);
    if (point_iter == m_spline->data().end())
        return false;

    m_spline->data().erase(point_iter);
    return true;
}

CurveView::SplineDataSet::iterator CurveView::findSplinePoint(PointId id) const
{
    return m_spline->data().get_point(id);
}

void CurveView::updateTransformation()
{
    // Set transformation
    RangeF valueRange = m_model->valueRange();
    float maxi = valueRange.max;
    float mini = valueRange.min;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);
}
