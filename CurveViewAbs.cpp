#include "CurveViewAbs.h"
#include "CurveModelAbs.h"
#include "PointView.h"
#include <QDebug>
#include <assert.h>

CurveViewAbs::CurveViewAbs(std::shared_ptr<CurveModelAbs> model, QGraphicsItem* parent)
  : TransformationNode(parent),
    m_model(model),
    m_snapGridRect(QRectF()),
    m_snapToGrid(false),
    m_highlightCurve(model->isSelected())
{
    // Connecting to model is done in init() to avoid virtual calls from constructor
}

void CurveViewAbs::init()
{
    connect(m_model.get(), &CurveModelAbs::selectedChanged, this, &CurveViewAbs::highlightCurve);
    connect(m_model.get(), &CurveModelAbs::pointAdded, this, &CurveViewAbs::addPoint);
    connect(m_model.get(), &CurveModelAbs::pointUpdated, this, &CurveViewAbs::updatePoint);
    connect(m_model.get(), &CurveModelAbs::pointRemoved, this, &CurveViewAbs::removePoint);

    for (auto pid : m_model->pointIds())
        addPoint(pid);
}

CurveViewAbs::~CurveViewAbs()
{
}

void CurveViewAbs::setSnapGrid(QRectF gridRect)
{
    if (m_snapGridRect != gridRect)
    {
        m_snapGridRect = gridRect;
        if (m_snapToGrid)
            emit snapGridChanged(getSnapGrid());
    }
}

void CurveViewAbs::setSnapToGrid(bool snapToGrid)
{
    if (m_snapToGrid != snapToGrid)
    {
        m_snapToGrid = snapToGrid;
        emit snapGridChanged(getSnapGrid());
    }
}


void CurveViewAbs::highlightCurve(bool highlight)
{
    if (m_highlightCurve != highlight)
    {
        m_highlightCurve = highlight;
        updateCurves();
    }
}

void CurveViewAbs::duplicateSelectedPoints()
{
    qDebug() << "CurveViewAbs::duplicateSelectedPoints";

    QSet<PointId> toBeDuplicated;

    for (auto &point : m_pointViews)
        if (point->isSelected())
            toBeDuplicated.insert(point->pointId());

    for (auto pid : toBeDuplicated)
    {
        std::pair<float, QVariant> timeValue = placeNewPoint(pid, m_model->nextPointId(pid));
        m_model->addPoint(timeValue.first, timeValue.second);
    }
}

void CurveViewAbs::removeSelectedPoints()
{
    qDebug() << "CurveViewAbs::removeSelectedPoints";

    QSet<PointId> toBeRemoved;

    for (auto &point : m_pointViews)
        if (point->isSelected())
            toBeRemoved.insert(point->pointId());

    for (auto pid : toBeRemoved)
        m_model->removePoint(pid);
}

void CurveViewAbs::addPoint(PointId id)
{
    qDebug() << "CurveViewAbs::addPoint" << id;
    assert(m_model);

    assert(id.isValid());
    assert(!findPointView(id));

    if (!internalAddPoint(id))
    {
        qWarning() << "Internal point add failed for" << id;
        return;
    }

    PointView* pointView = new PointView(m_model->point(id), this);
    m_pointViews.insert(id, pointView);
    connect(pointView, &PointView::pointPositionChanged, m_model.get(), &CurveModel::updatePoint);
    connect(pointView, &PointView::pointSelectedChanged, m_model.get(), &CurveModel::pointSelectedChanged);
    connect(this, &CurveViewAbs::snapGridChanged, pointView, &PointView::setSnapGrid);
    pointView->setSnapGrid(getSnapGrid());

    updateCurves();
}

void CurveViewAbs::updatePoint(PointId id)
{
    qDebug() << "CurveView::updatePoint" << id;
    assert(id.isValid());

    PointView* pointView = findPointView(id);
    assert(pointView);

    if (!internalUpdatePoint(id))
    {
        qWarning() << "Internal point update failed for" << id;
        return;
    }

    pointView->setPoint(m_model->point(id));

    updateCurves();
}

void CurveViewAbs::removePoint(PointId id)
{
    qDebug() << "CurveView::removePoint" << id;

    if (!internalRemovePoint(id))
    {
        qWarning() << "Internal point update failed for" << id;
        // Point will be deleted anyways
    }

    PointView* pointView = findPointView(id);
    assert(pointView);
    delete pointView;

    int numberOfRemoved = m_pointViews.remove(id);
    assert(numberOfRemoved == 1);

    updateCurves();
}

QRectF CurveViewAbs::getSnapGrid() const
{
    return m_snapToGrid ? m_snapGridRect : QRectF();
}

bool CurveViewAbs::isHighlighted() const
{
    return m_highlightCurve;
}

PointView* CurveViewAbs::findPointView(PointId id) const
{
    return m_pointViews.value(id, nullptr);
}
