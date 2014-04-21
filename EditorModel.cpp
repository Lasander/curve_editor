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

EditorModel::EditorModel(RangeF timeRange, float beatOffset, float bpm)
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

float EditorModel::beatOffset() const
{
    return m_beatOffset;
}

float EditorModel::bpm() const
{
    return m_bpm;
}

void EditorModel::addCurve(std::shared_ptr<CurveModel> model)
{
    if (m_curves.contains(model))
        return;

    m_curves.push_back(model);
    emit curveAdded(model);
}

void EditorModel::removeCurve(std::shared_ptr<CurveModel> model)
{
    int removed = m_curves.removeAll(model);
    if (removed < 1)
        return;

    emit curveRemoved(model);
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

void EditorModel::setBeatOffset(float beatOffset)
{
    if (m_beatOffset != beatOffset)
    {
        m_beatOffset = beatOffset;
        emit beatOffsetChanged(m_beatOffset);
    }
}

void EditorModel::setBpm(float bpm)
{
    if (m_bpm != bpm)
    {
        m_bpm = bpm;
        emit bpmChanged(m_bpm);
    }
}

