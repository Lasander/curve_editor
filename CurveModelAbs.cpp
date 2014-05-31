#include "CurveModelAbs.h"
#include <QDebug>

CurveModelAbs::CurveModelAbs(const QString& name)
  : m_name(name),
    m_selected(false),
    m_timeRange()
{
}

CurveModelAbs::~CurveModelAbs()
{
}

const QString& CurveModelAbs::name() const
{
    return m_name;
}

bool CurveModelAbs::isSelected() const
{
    return m_selected;
}

RangeF CurveModelAbs::timeRange() const
{
    return m_timeRange;
}

void CurveModelAbs::setName(QString name)
{
    m_name = name;
}

void CurveModelAbs::setSelected(bool status)
{
    if (m_selected != status)
    {
        m_selected = status;
        emit selectedChanged(m_selected);
    }
}

void CurveModelAbs::setTimeRange(RangeF newRange)
{
    if (m_timeRange != newRange)
    {
        m_timeRange = newRange;
        forcePointsToTimeRange(m_timeRange);
        emit timeRangeChanged(m_timeRange);
    }
}

QList<PointId> CurveModelAbs::pointIds() const
{
    QList<PointId> output;

    for (auto &p : m_points)
        output.push_back(p.id());

    return std::move(output);
}

PointId CurveModelAbs::nextPointId(PointId id) const
{
    PointContainer::ConstIterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Trying to get for unknown point id:" << id;
        return PointId::invalidId();
    }

    // Get next
    ++it;
    if (it == m_points.end())
    {
        // No next point
        return PointId::invalidId();
    }

    return it->id();
}

const Point CurveModelAbs::point(PointId id) const
{
    PointContainer::ConstIterator it = findPoint(id);
    if (it == m_points.end())
        return Point();

    return *it;
}

int CurveModelAbs::numberOfPoints() const
{
    return m_points.size();
}

void CurveModelAbs::addPoint(float time, QVariant value)
{
    // Add to point container
    value = limitValueToRange(value);
    Point p(time, value, false);

    if (!addPointInternal(p.id(), time, value))
        return;

    PointContainer::Iterator pointIt = m_points.insert(time, p);
    if (pointIt == m_points.end())
    {
        qWarning() << "Point insert failed" << p.id() << time;
        removePointInternal(p.id());
        return;
    }

    emit pointAdded(p.id());
}

void CurveModelAbs::updatePoint(PointId id, float time, QVariant value)
{
    PointContainer::Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }

    time = limitTimeToRange(time);
    value = limitValueToRange(value);

    const Point old = *it;
    Point p(time, value, old.isSelected(), old.id());
    if (p == old)
        return; // No change

    // Remove/add to also update key (keep sorted)
    m_points.erase(it);
    m_points.insert(time, p);

    emit pointUpdated(id);
}

void CurveModelAbs::pointSelectedChanged(PointId id, bool isSelected)
{
    qDebug() << "Point:" << id << ":" <<  (isSelected ? "selected" : "deselected");

    PointContainer::Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point selected changed" << id;
        return;
    }

    const bool oldSelected = it->isSelected();
    it->setSelected(isSelected);
    const bool newSelected = it->isSelected();

    if (newSelected && !oldSelected)
    {
        emit pointSelected(id);
    }
    else if (!newSelected && oldSelected)
    {
        emit pointDeselected(id);
    }
}

void CurveModelAbs::removePoint(PointId id)
{
    PointContainer::Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }

    // Deselect point before removing
    if (it->isSelected())
        emit pointDeselected(id);

    m_points.erase(it);
    removePointInternal(id);

    emit pointRemoved(id);
}

CurveModelAbs::PointContainer::Iterator CurveModelAbs::findPoint(PointId id)
{
    PointContainer::Iterator it = m_points.begin();
    for (; it != m_points.end(); ++it)
    {
        if ((*it).id() == id)
            break;
    }
    return it;
}

CurveModelAbs::PointContainer::ConstIterator CurveModelAbs::findPoint(PointId id) const
{
    PointContainer::ConstIterator it = m_points.begin();
    for (; it != m_points.end(); ++it)
    {
        if ((*it).id() == id)
            break;
    }
    return it;
}

bool CurveModelAbs::addPointInternal(PointId id, float time, QVariant value)
{
    Q_UNUSED(id) Q_UNUSED(time) Q_UNUSED(value)
    return true;
}

void CurveModelAbs::removePointInternal(PointId id)
{
    Q_UNUSED(id)
}

QVariant CurveModelAbs::limitValueToRange(const QVariant& value) const
{
    return value;
}

void CurveModelAbs::forcePointsToTimeRange(RangeF newRange)
{
    // Ensure all points fit to the new time range
    for (auto pid : pointIds())
    {
        PointContainer::Iterator it = findPoint(pid);
        if (it->time() < newRange.min || it->time() > newRange.max)
            updatePoint(pid, it->time(), it->value()); // updatePoint will do necessary clamping
    }
}

float CurveModelAbs::limitTimeToRange(float time) const
{
    return timeRange().clampToRange(time);
}

