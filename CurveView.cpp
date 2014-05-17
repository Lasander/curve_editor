#include "CurveView.h"
#include "PointView.h"
#include <QGraphicsPathItem>
#include <QDebug>
#include <QPen>
#include <assert.h>

CurveView::CurveView(std::shared_ptr<CurveModel> model, QGraphicsItem* parent)
:   CurveViewAbs(model, parent),
    m_model(model)
{
    m_spline = new Spline;
    m_curveView = new QGraphicsPathItem(this);
    
    connect(m_model.get(), &CurveModel::valueRangeChanged, this, &CurveView::changeValueRange);
    updateTransformation();
}

CurveView::~CurveView()
{
}

CurveView* CurveView::create(std::shared_ptr<CurveModel> model, QGraphicsItem* parent)
{
    CurveView* curveView = new CurveView(model, parent);
    curveView->init();
    return curveView;
}

std::pair<float, QVariant> CurveView::placeNewPoint(PointId id, PointId nextId) const
{
    const Point point = m_model->point(id);

    if (nextId.isValid())
    {
        const Point nextPoint = m_model->point(nextId);
        float insertTime = (point.time() + nextPoint.time()) / 2.0f;
        float insertValue = m_spline->value_at(insertTime);
        return std::make_pair(insertTime, QVariant(insertValue));
    }

    // No next point, add point to +1 second from the original point with the same value(s)
    return std::make_pair(point.time() + 1.0, point.value());
}

bool CurveView::internalAddPoint(PointId id)
{
    return addToSpline(id);
}

bool CurveView::internalUpdatePoint(PointId id)
{
    const bool removeOk = removeFromSpline(id);
    const bool addOk = addToSpline(id);
    return removeOk && addOk;
}

bool CurveView::internalRemovePoint(PointId id)
{
    return removeFromSpline(id);
}

void CurveView::changeValueRange(RangeF /*valueRange*/)
{
    updateTransformation();
}

void CurveView::updateCurves()
{
    static const float ESPILON = 0.1;

    QPen curvePen(Qt::blue);
    curvePen.setCosmetic(true);
    if (isHighlighted())
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
