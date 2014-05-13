#include "CurveModelAbs.h"

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

