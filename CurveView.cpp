//
//  CurveView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#include "CurveView.h"
#include "PointView.h"
#include "ScaleView.h"
#include <QGraphicsPathItem>
#include <QDebug>
#include <QPen>
#include <assert.h>

CurveView::CurveView(std::shared_ptr<CurveModel> model, QGraphicsItem* parent)
:   TransformationNode(parent),
	m_model(model)
{
	for (int i = 0; i < model->dimension(); ++i)
    {
        Spline* spline = new Spline(SplineDataSet());
		m_splines.insert(i, spline);
        
        QGraphicsPathItem* curveItem = new QGraphicsPathItem(this);
        QPen curvePen(Qt::blue);
        curvePen.setCosmetic(true);
        curveItem->setPen(curvePen);
        
        m_curveViews[spline] = curveItem;
    }
    
    // Set transformation
    
    connect(m_model.get(), &CurveModel::pointAdded, this, &CurveView::pointAdded);
    connect(m_model.get(), &CurveModel::pointUpdated, this, &CurveView::pointUpdated);
    connect(m_model.get(), &CurveModel::pointRemoved, this, &CurveView::pointRemoved);
    
    for (auto pid : m_model->pointIds())
    	pointAdded(pid);


    connect(m_model.get(), &CurveModel::valueRangeChanged, this, &CurveView::valueRangeChanged);
    updateTransformation();
}

CurveView::~CurveView()
{
	for (auto p : m_splines)
		delete p;
}

void CurveView::pointAdded(PointId id)
{
    qDebug() << "CurveView::pointAdded" << id;
    assert(m_model);
    
    const CurveModel::Point point = m_model->point(id);
    assert(point.is_valid());
    assert(findPointView(id).isEmpty());

    for (int i = 0; i < m_model->dimension(); ++i)
    {
    	PointView* pointView = new PointView(point, i, this);
        m_pointViews.insert(id, pointView);
        connect(pointView, &PointView::pointPositionChanged, m_model.get(), &CurveModel::updatePoint);
    }

    bool addOk = addToSpline(point);
    assert(addOk);
    
    updateCurves();
}

void CurveView::pointUpdated(PointId id)
{
    qDebug() << "CurveView::pointUpdated" << id;
    
    QList<PointView*> pointViews = findPointView(id);
    assert(!pointViews.isEmpty());
    const CurveModel::Point old(pointViews[0]->point());
    
    const CurveModel::Point p = m_model->point(id);
    assert(p.is_valid());

    for (int i = 0; i < m_model->dimension(); ++i)
	    pointViews[i]->setPoint(p);

	bool removeOk = removeFromSpline(old);
    assert(removeOk);
    
    bool addOk = addToSpline(p);
    assert(addOk);
    
    updateCurves();
}

void CurveView::pointRemoved(PointId id)
{
    qDebug() << "CurveView::pointRemoved" << id;
    
    QList<PointView*> pointViews = findPointView(id);
    assert(!pointViews.isEmpty());
    const CurveModel::Point removed(pointViews[0]->point());

    int numberOfRemoved = m_pointViews.remove(id);
    assert(numberOfRemoved == m_model->dimension());
    
    for (int i = 0; i < m_model->dimension(); ++i)
    	delete pointViews[i];
    
	bool removeOk = removeFromSpline(removed);
    assert(removeOk);
    
  	updateCurves();
}

void CurveView::duplicateSelectedPoints()
{
    qDebug() << "CurveView::duplicateSelectedPoints";

    QSet<PointId> toBeDuplicated;
    
    for (auto &point : m_pointViews)
    	if (point->isSelected())
            toBeDuplicated.insert(point->point().id());
    
    for (auto pid : toBeDuplicated)
    {
        const CurveModel::Point point = m_model->point(pid);

        const PointId nextPointId = m_model->nextPointId(pid);
        if (isValid(nextPointId))
        {
            // Add point to between this and the next point matching the curve value at that point
            const CurveModel::Point nextPoint = m_model->point(nextPointId);
            float insertTime = (point.time() + nextPoint.time()) / 2.0f;

            QList<float> insertValues;
            for (int dim = 0; dim < m_splines.size(); ++dim)
            {
                insertValues.push_back(m_splines[dim]->value_at(insertTime));
            }
            m_model->addPoint(insertTime, insertValues);
        }
        else
        {
            // Add point to +1 second from the original point with the same value(s)
            m_model->addPoint(point.time() + 1.0, point.values());
        }
    }
}

void CurveView::removeSelectedPoints()
{
    QSet<PointId> toBeRemoved;
    
    for (auto &point : m_pointViews)
    	if (point->isSelected())
            toBeRemoved.insert(point->point().id());

    for (auto pid : toBeRemoved)
        m_model->removePoint(pid);
}

void CurveView::valueRangeChanged(RangeF /*valueRange*/)
{
    updateTransformation();
}

void CurveView::selectedChanged(bool /*status*/)
{
    // TODO
}

QList<PointView*> CurveView::findPointView(PointId id)
{
    return m_pointViews.values(id);
}

void CurveView::updateCurves()
{
    static const float ESPILON = 0.1;

    for (auto spline : m_splines)
    {
        auto cur = spline->data().begin();
        if (cur == spline->data().end())
        {
            // No spline data
            m_curveViews[spline]->setPath(QPainterPath());
            continue;
        }
        
        constexpr int STEPS_PER_INTERVAL = 20;
        
        // Start by moving tot the first point
        QPainterPath path;
        path.moveTo(QPointF(cur->time(), cur->value()));
        
        auto next = cur + 1;
        while (next != spline->data().end())
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
                    path.lineTo(QPointF(time, spline->value_at(time)));
                }
            }
            
            // End the curve section to the next point
            path.lineTo(QPointF(endTime, next->value()));
            ++cur;
            ++next;
        }
        
        m_curveViews[spline]->setPath(path);
    }
}

bool CurveView::addToSpline(CurveModel::Point const& point)
{
    if (!point.is_valid())
        return false;
    
    pt::math::kochanek_bartels_parameters params(point.tension(), point.bias(), point.continuity());
    
	for (int i = 0; i < m_model->dimension(); ++i)
    {
        pt::math::kb_data_set<float>::point p(point.id(), point.time(), point.value(i), params);
        auto point_iter = m_splines[i]->data().add(p);
        if (point_iter == m_splines[i]->data().end())
            return false;
    }
    
    return true;
}

bool CurveView::removeFromSpline(CurveModel::Point const& point)
{
    if (!point.is_valid())
        return false;
    
	for (int i = 0; i < m_model->dimension(); ++i)
    {
        auto point_iter = findSplinePoint(point, i);
        if (point_iter == m_splines[i]->data().end())
            return false;
        
        m_splines[i]->data().erase(point_iter);
    }
    
    return true;
}

CurveView::SplineDataSet::const_iterator CurveView::findSplinePoint(CurveModel::Point const& point, int index)
{
    return m_splines[index]->data().get_point(point.id());
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
