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

CurveModel::Point::Point()
:	m_isValid(false), m_isSelected(false), m_id(PointId::invalidId()), m_time(0), m_value(0),
	m_tension(0), m_bias(0), m_continuity(0)
{
}

CurveModel::Point::Point(float time, float value, bool isSelected, PointId id)
:	m_isValid(true), m_isSelected(isSelected), m_id(id.isValid() ? id : PointId::generateId()), m_time(time), m_value(value),
	m_tension(0), m_bias(0), m_continuity(0)
{
}

CurveModel::Point::Point(float time, float value, float tension, float bias, float continuity, bool isSelected, PointId id)
:	m_isValid(true), m_isSelected(isSelected), m_id(id.isValid() ? id : PointId::generateId()), m_time(time), m_value(value),
	m_tension(tension), m_bias(bias), m_continuity(continuity)
{
}

void CurveModel::Point::updateParams(float tension, float bias, float continuity)
{
	m_tension = tension;
	m_bias = bias;
	m_continuity = continuity;
}

bool CurveModel::Point::isSelected() const
{
    return m_isSelected;
}

void CurveModel::Point::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}

/////////////////////////////////////////
/////////////////////////////////////////

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
    ConstIterator it = findPoint(id);
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

const CurveModel::Point CurveModel::point(PointId id) const
{
    ConstIterator it = findPoint(id);
    if (it == m_points.end())
        return Point();
    
    return *it;
}

int CurveModel::numberOfPoints() const
{
    return m_points.size();
}

void CurveModel::addPoint(float time, float value)
{
    addPoint(time, value, 0, 0, 0);
}

void CurveModel::addPoint(float time, float value, float tension, float bias, float continuity)
{
    time = limitTimeToRange(time);
    value = limitValueToScale(value);

    Point p(time, value, tension, bias, continuity, false);
    m_points.insert(time, p);
    
    emit pointAdded(p.id());
}

void CurveModel::updatePoint(PointId id, float time, float value)
{
    Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }
    
    time = limitTimeToRange(time);
    value = limitValueToScale(value);
    
    const Point old = *it;
    Point p(time, value, old.tension(), old.bias(), old.continuity(), old.isSelected(), old.id());
    if (p == old)
        return; // No change
    
    m_points.erase(it);
    m_points.insert(time, p);
    
    emit pointUpdated(id);
}

void CurveModel::updatePointParams(PointId id, float tension, float bias, float continuity)
{
    qDebug() << "updatePointParams" << id << tension << bias << continuity;

    Iterator it = findPoint(id);
    if (it == m_points.end())
        return;
    
    Point p(it->time(), it->value(), tension, bias, continuity, it->isSelected(), it->id());
    if (p == *it)
        return; // No change
    
    it->updateParams(tension, bias, continuity);
    emit pointUpdated(id);
}

void CurveModel::pointSelectedChanged(PointId id, bool isSelected)
{
    qDebug() << "Point:" << id << ":" <<  (isSelected ? "selected" : "deselected");

    Iterator it = findPoint(id);
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
    Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }
    
    // Deselect point before removing
    if (it->isSelected())
        emit pointDeselected(id);

    m_points.erase(it);
    
    emit pointRemoved(id);
}

void CurveModel::forcePointsToTimeRange(RangeF newRange)
{
    // Ensure all points fit to the new time range
    for (auto pid : pointIds())
    {
        Iterator it = findPoint(pid);
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
            Iterator it = findPoint(pid);
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

CurveModel::Iterator CurveModel::findPoint(PointId id)
{
    Iterator it = m_points.begin();
    for (; it != m_points.end(); ++it)
    {
        if ((*it).id() == id)
            break;
    }
    return it;
}

CurveModel::ConstIterator CurveModel::findPoint(PointId id) const
{
    ConstIterator it = m_points.begin();
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

float CurveModel::limitValueToScale(float value) const
{
    return m_valueRange.clampToRange(value);
}
