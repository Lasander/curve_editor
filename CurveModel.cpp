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


const CurveModel::KbParams CurveModel::params(PointId id) const
{
    ParamContainer::ConstIterator it = m_params.find(id);
    if (it == m_params.end())
        return KbParams();

    return *it;
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

bool CurveModel::addPointInternal(PointId id, float time, QVariant value)
{
    Q_UNUSED(time)
    Q_UNUSED(value)

    // Add point to param container
    ParamContainer::Iterator paramIt = m_params.insert(id, KbParams());
    if (paramIt == m_params.end())
        return false;

    return true;
}

void CurveModel::removePointInternal(PointId id)
{
    int removedParams = m_params.remove(id);
    if (removedParams != 1)
        qWarning() << "Unable to remove parameters for " << id;
}

void CurveModel::setValueRange(RangeF newRange)
{
    if (m_valueRange != newRange)
    {
        m_valueRange = newRange;

        // Ensure all points fit to the new value range
        for (auto pid : pointIds())
        {
            const Point p = point(pid);
            if (p.value() < m_valueRange.min || p.value() > m_valueRange.max)
            {
                updatePoint(pid, p.time(), p.value()); // updatePoint will do necessary clamping
            }
        }

        emit valueRangeChanged(m_valueRange);
    }
}

RangeF CurveModel::valueRange() const
{
    return m_valueRange;
}

QVariant CurveModel::limitValueToRange(const QVariant& value) const
{
    return m_valueRange.clampToRange(value.toFloat());
}
