#include "StepCurveView.h"
#include "PointView.h"
#include <QGraphicsPathItem>
#include <QDebug>
#include <QPen>
#include <assert.h>

StepCurveView::StepCurveView(std::shared_ptr<StepCurveModel> model, QGraphicsItem* parent)
:   CurveViewAbs(model, parent),
    m_model(model)
{
    m_curveView = new QGraphicsPathItem(this);
    connect(m_model.get(), &StepCurveModel::optionsChanged, this, &StepCurveView::updateOptions);

    updateTransformation();
}

StepCurveView::~StepCurveView()
{
}

StepCurveView* StepCurveView::create(std::shared_ptr<StepCurveModel> model, QGraphicsItem* parent)
{
    StepCurveView* curveView = new StepCurveView(model, parent);
    curveView->init();
    return curveView;
}

void StepCurveView::updateOptions(StepCurveModel::Options options)
{
    Q_UNUSED(options)
    updateTransformation();
}

bool StepCurveView::internalAddPoint(PointId id)
{
    Q_UNUSED(id)
    return true;
}

bool StepCurveView::internalUpdatePoint(PointId id)
{
    Q_UNUSED(id)
    return true;
}

bool StepCurveView::internalRemovePoint(PointId id)
{
    Q_UNUSED(id)
    return true;
}

std::pair<float, QVariant> StepCurveView::placeNewPoint(PointId id, PointId nextId) const
{
    const Point point = m_model->point(id);

    if (nextId.isValid())
    {
        const Point nextPoint = m_model->point(nextId);
        float insertTime = (point.time() + nextPoint.time()) / 2.0f;
        return std::make_pair(insertTime, point.value());
    }

    // No next point, add point to +1 second from the original point with the same value(s)
    return std::make_pair(point.time() + 1.0, point.value());
}

void StepCurveView::updateCurves()
{
    QPen curvePen(Qt::blue);
    curvePen.setCosmetic(true);
    if (isHighlighted())
    {
        // Highlight curve by boosting width and darkening color
        curvePen.setWidth(curvePen.width() + 1);
        curvePen.setColor(curvePen.color().darker(100));
    }

    QList<PointId> ids = m_model->pointIds();
    if (ids.isEmpty())
    {
        // No points
        m_curveView->setPath(QPainterPath());
        return;
    }

//    const QList<int> optionKeys = m_model->options().keys();

    auto cur = ids.begin();

    // Start by moving to the first point
    QPainterPath path;
    Point currentPoint = m_model->point(*cur);
    path.moveTo(QPointF(currentPoint.time(), currentPoint.value().toInt()));

    auto next = cur + 1;
    while (next != ids.end())
    {
        const Point nextPoint = m_model->point(*next);
        const float endTime = nextPoint.time();

        // Draw horizontal line to from current point to next point time
        path.lineTo(QPointF(endTime, currentPoint.value().toInt()));
        // Draw vertical line from current point value to next point value at next point time
        path.lineTo(QPointF(endTime, nextPoint.value().toInt()));

        // Move to next point
        currentPoint = nextPoint;
        ++cur;
        ++next;
    }

    m_curveView->setPen(curvePen);
    m_curveView->setPath(path);
}

void StepCurveView::updateTransformation()
{
    // Set transformation
    StepCurveModel::Options options = m_model->options();

    float maxi = options.lastKey() + 1;
    float mini = options.firstKey() - 1;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);
}
