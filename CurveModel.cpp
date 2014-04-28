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

namespace {
    
class TimeRangeChangeChecker
{
public:
    TimeRangeChangeChecker(CurveModel& container)
    : 	m_container(container),
    	m_startRange(container.timeRange())
    {
    }
    
    ~TimeRangeChangeChecker()
    {
        RangeF endRange = m_container.timeRange();
        if (endRange != m_startRange)
        {
			m_container.timeRangeChanged(endRange);
        }
    }
    
private:
    CurveModel& m_container;
    RangeF m_startRange;
};

} // anonymous namespace

/////////////////////////////////////////
/////////////////////////////////////////

bool isValid(PointId pointId)
{
    // Zero is invalid
    return pointId != 0;
}

/////////////////////////////////////////
/////////////////////////////////////////


CurveModel::Point::Point()
:	m_is_valid(false), m_id(0), m_time(0), m_values({0}),
	m_tension(0), m_bias(0), m_continuity(0)
{
}

CurveModel::Point::Point(float time, QList<float> values, PointId id)
:	m_is_valid(true), m_id(id == 0 ? generateId() : id), m_time(time), m_values(values),
	m_tension(0), m_bias(0), m_continuity(0)
{
}

CurveModel::Point::Point(float time, QList<float> values, float tension, float bias, float continuity, PointId id)
:	m_is_valid(true), m_id(id == 0 ? generateId() : id), m_time(time), m_values(values),
	m_tension(tension), m_bias(bias), m_continuity(continuity)
{
}

void CurveModel::Point::updateParams(float tension, float bias, float continuity)
{
	m_tension = tension;
	m_bias = bias;
	m_continuity = continuity;
}

PointId CurveModel::Point::generateId()
{
    static PointId g_id = 1;
    PointId id = ++g_id;
    
    // Skip 0 as invalid id
    if (id == 0)
        return generateId();
    
    return id;
}

PointId CurveModel::Point::invalidId()
{
    return 0;
}

/////////////////////////////////////////
/////////////////////////////////////////

CurveModel::CurveModel(int dimension, const QString& name)
:	m_dimension(dimension),
    m_timeRange(),
    m_valueRange(-100, 100),
    m_selected(false),
    m_name(name)
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
        return Point::invalidId();
    }

    // Get next
    ++it;
    if (it == m_points.end())
    {
        // No next point
        return Point::invalidId();
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

int CurveModel::dimension() const
{
    return m_dimension;
}

void CurveModel::addPoint(float time, QList<float> values)
{
	addPoint(time, values, 0, 0, 0);
}

void CurveModel::addPoint(float time, QList<float> values, float tension, float bias, float continuity)
{
    if (values.size() != m_dimension)
    {
        qWarning() << "Incorrect value dimension:" << values.size() << "expected:" << m_dimension;
        return;
    }

    TimeRangeChangeChecker rangeCheck(*this);
    
    time = limitTimeToRange(time);
    values = limitValuesToScale(values);
    
    Point p(time, values, tension, bias, continuity);
    m_points.insert(time, p);
    
    emit pointAdded(p.id());
}

void CurveModel::updatePoint(PointId id, float time, float value, int index)
{
    if (index < 0 || index >= m_dimension)
    {
        qWarning() << "Incorrect point dimension:" << index<< "expected: [ 0 -" << m_dimension - 1 << "]";
        return;
    }
    
    TimeRangeChangeChecker rangeCheck(*this);

    Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }
    
    time = limitTimeToRange(time);
    value = limitValueToScale(value);
    
    const Point old = *it;
    QList<float> values(old.values());
    values[index] = value;
    
    Point p(time, values, old.tension(), old.bias(), old.continuity(), old.id());
    if (p == old)
        return; // No change
    
    m_points.erase(it);
    m_points.insert(time, p);
    
    emit pointUpdated(id);
}

void CurveModel::updatePointParams(PointId id, float tension, float bias, float continuity)
{
    Iterator it = findPoint(id);
    if (it == m_points.end())
        return;
    
    Point p(it->time(), it->values(), tension, bias, continuity, it->id());
    if (p == *it)
        return; // No change
    
    it->updateParams(tension, bias, continuity);
    emit pointUpdated(id);
}

void CurveModel::removePoint(PointId id)
{
    TimeRangeChangeChecker rangeCheck(*this);

    Iterator it = findPoint(id);
    if (it == m_points.end())
    {
        qWarning() << "Unknown point" << id;
        return;
    }
    
    m_points.erase(it);
    
    emit pointRemoved(id);
}

void CurveModel::setTimeRange(RangeF newRange)
{
    if (m_timeRange != newRange)
    {
        m_timeRange = newRange;

        // Ensure all points fit to the new time range
        for (auto pid : pointIds())
        {
            Iterator it = findPoint(pid);
            if (it->time() < m_timeRange.min || it->time() > m_timeRange.max)
            {
                for (int d = 0; d < m_dimension; ++d)
                {
                    updatePoint(pid, it->time(), it->value(d), d); // updatePoint will do necessary clamping
                    it = findPoint(pid); // re-validate iterator
                }
            }
        }

        emit timeRangeChanged(m_timeRange);
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
            for (int d = 0; d < m_dimension; ++d)
            {
                if (it->value(d) < m_valueRange.min || it->value(d) > m_valueRange.max)
                {
                    updatePoint(pid, it->time(), it->value(d), d); // updatePoint will do necessary clamping
                    it = findPoint(pid); // re-validate iterator
                }
            }
        }

        emit valueRangeChanged(m_valueRange);
    }
}

void CurveModel::setSelected(bool status)
{
    if (m_selected != status)
    {
        m_selected = status;
        emit selectedChanged(m_selected);
    }
}

RangeF CurveModel::timeRange() const
{
    return m_timeRange;
}

RangeF CurveModel::valueRange() const
{
    return m_valueRange;
}

bool CurveModel::isSelected() const
{
    return m_selected;
}

const QString& CurveModel::name() const
{
    return m_name;
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
    return m_timeRange.clampToRange(time);
}

float CurveModel::limitValueToScale(float value) const
{
    return m_valueRange.clampToRange(value);
}

QList<float> CurveModel::limitValuesToScale(QList<float> values) const
{
	for (int i = 0; i < values.size(); ++i)
        values[i] = m_valueRange.clampToRange(values[i]);
    
    return values;
}

