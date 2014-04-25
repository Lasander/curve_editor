//
//  EditorModel.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 31.12.13.
//
//

#include "EditorModel.h"
#include "CurveModel.h"
#include <QDebug>

EditorModel::EditorModel(RangeF timeRange, double beatOffset, double bpm)
  : m_timeRange(timeRange),
    m_beatOffset(beatOffset),
    m_bpm(bpm)
{
}

EditorModel::~EditorModel()
{
}

QList<std::shared_ptr<CurveModel>> EditorModel::curves() const
{
    return m_curves;
}

const RangeF EditorModel::timeRange() const
{
    return m_timeRange;
}

double EditorModel::beatOffset() const
{
    return m_beatOffset;
}

double EditorModel::bpm() const
{
    return m_bpm;
}

void EditorModel::addCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to add bad curve";
        return;
    }

    if (m_curves.contains(curve))
    {
        qWarning() << "Trying to add duplicate curve:" << curve->name();
        return;
    }

    m_curves.push_back(curve);
    emit curveAdded(curve);
}

void EditorModel::removeCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to remove bad curve";
        return;
    }

    int removed = m_curves.removeAll(curve);
    if (removed < 1)
    {
        qWarning() << "Trying to remove non-existent curve:" << curve->name();
        return;
    }

    emit curveRemoved(curve);
}

void EditorModel::clearCurves()
{
    for (auto curve : m_curves)
        removeCurve(curve);
}

void EditorModel::setTimeRange(RangeF timeRange)
{
    if (m_timeRange != timeRange)
    {
        m_timeRange = timeRange;
        emit timeRangeChanged(m_timeRange);
    }
}

void EditorModel::setBeatOffset(double beatOffset)
{
    if (m_beatOffset != beatOffset)
    {
        m_beatOffset = beatOffset;
        emit beatOffsetChanged(m_beatOffset);
    }
}

void EditorModel::setBpm(double bpm)
{
    if (m_bpm != bpm)
    {
        m_bpm = bpm;
        emit bpmChanged(m_bpm);
    }
}

