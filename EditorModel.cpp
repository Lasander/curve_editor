#include "EditorModel.h"
#include "CurveModel.h"
#include "StepCurveModel.h"
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

QList<std::shared_ptr<CurveModelAbs>> EditorModel::curves() const
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

void EditorModel::addCurve(std::shared_ptr<CurveModelAbs> curve)
{
    if (!addCurveInternal(curve))
        return;

    emit curveAdded(curve);
}

bool EditorModel::addCurveInternal(std::shared_ptr<CurveModelAbs> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to add bad curve";
        return false;
    }

    if (m_curves.contains(curve))
    {
        qWarning() << "Trying to add duplicate curve:" << curve->name();
        return false;
    }

    m_curves.push_back(curve);
    return true;
}

void EditorModel::removeCurve(std::shared_ptr<CurveModelAbs> curve)
{
    if (!removeCurveInternal(curve))
        return;

    emit curveRemoved(curve);
}

bool EditorModel::removeCurveInternal(std::shared_ptr<CurveModelAbs> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to remove bad curve";
        return false;
    }

    int removed = m_curves.removeAll(curve);
    if (removed < 1)
    {
        qWarning() << "Trying to remove non-existent curve:" << curve->name();
        return false;
    }

    return true;
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

void EditorModel::handleRequestToAddNewCurve(std::shared_ptr<CurveModelAbs> curve)
{
    emit requestToAddNewCurve(curve);
}
