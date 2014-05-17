//
//  CurveModel.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#include "CurveModel.h"
#include <assert.h>
#include <utility>

CurveModel::CurveModel(const QString& name)
:	CurveModelAbs(name),
    m_valueRange(-100, 100)
{
}

CurveModel::~CurveModel()
{
}

QList<PointId> CurveModel::pointIds() const
{
    QList<PointId> output;
    
	for (auto &p : m_points)
        output.push_back(p.id());
    
    return std::move(output);
}

PointId CurveModel::nextPointId(PointId id) const
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

const Point CurveModel::point(PointId id) const
{
    PointContainer::ConstIterator it = findPoint(id);
    if (it == m_points.end())
        return Point();
    
    return *it;
}

const CurveModel::KbParams CurveModel::params(PointId id) const
{
    ParamContainer::ConstIterator it = m_params.find(id);
    if (it == m_params.end())
        return KbParams();

    return *it;
}

int CurveModel::numberOfPoints() const
{
    return m_points.size();
}

void CurveModel::addPoint(float time, QVariant value)
{
    // Add to point container
    const QVariant limitedValue = limitValueToScale(value);
    Point p(time, limitedValue, false);
    PointContainer::Iterator pointIt = m_points.insert(time, p);
    if (pointIt == m_points.end())
        return;

    // Add to param container
    ParamContainer::Iterator paramIt = m_params.insert(p.id(), KbParams());
    if (paramIt == m_params.end())
    {
        m_points.erase(pointIt);
        return;
    }

    emit pointAdded(p.id());
}

void CurveModel::updatePoint(PointId id, float time, QVariant value)
{
    PointContainer::Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }

    time = limitTimeToRange(time);
    value = limitValueToScale(value);

    const Point old = *it;
    Point p(time, value, old.isSelected(), old.id());
    if (p == old)
        return; // No change

    // Remove/add to sort automatically
    m_points.erase(it);
    m_points.insert(time, p);

    emit pointUpdated(id);
}

void CurveModel::updatePointParams(PointId id, float tension, float bias, float continuity)
{
    qDebug() << "updatePointParams" << id << tension << bias << continuity;

    ParamContainer::Iterator it = m_params.find(id);
    if (it == m_params.end())
        return;
    
    const KbParams p(tension, bias, continuity);
    if (p != *it)
    {
        // Params changed
        (*it) = p;
        emit pointUpdated(id);
    }
}

void CurveModel::pointSelectedChanged(PointId id, bool isSelected)
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

void CurveModel::removePoint(PointId id)
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
    int removedParams = m_params.remove(id);
    assert(removedParams == 1);

    emit pointRemoved(id);
}

void CurveModel::forcePointsToTimeRange(RangeF newRange)
{
    // Ensure all points fit to the new time range
    for (auto pid : pointIds())
    {
        PointContainer::Iterator it = findPoint(pid);
        if (it->time() < newRange.min || it->time() > newRange.max)
            updatePoint(pid, it->time(), it->value()); // updatePoint will do necessary clamping
    }
}

void CurveModel::setValueRange(RangeF newRange)
{
    if (m_valueRange != newRange)
    {
        m_valueRange = newRange;

        // Ensure all points fit to the new value range
        for (auto pid : pointIds())
        {
            PointContainer::Iterator it = findPoint(pid);
            if (it->value() < m_valueRange.min || it->value() > m_valueRange.max)
            {
                updatePoint(pid, it->time(), it->value()); // updatePoint will do necessary clamping
            }
        }

        emit valueRangeChanged(m_valueRange);
    }
}

RangeF CurveModel::valueRange() const
{
    return m_valueRange;
}

CurveModel::PointContainer::Iterator CurveModel::findPoint(PointId id)
{
    PointContainer::Iterator it = m_points.begin();
    for (; it != m_points.end(); ++it)
    {
        if ((*it).id() == id)
            break;
    }
    return it;
}

CurveModel::PointContainer::ConstIterator CurveModel::findPoint(PointId id) const
{
    PointContainer::ConstIterator it = m_points.begin();
    for (; it != m_points.end(); ++it)
    {
        if ((*it).id() == id)
            break;
    }
    return it;
}

float CurveModel::limitTimeToRange(float time) const
{
    return timeRange().clampToRange(time);
}


QVariant CurveModel::limitValueToScale(const QVariant& value) const
{
    return m_valueRange.clampToRange(value.toFloat());
}
