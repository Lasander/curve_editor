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

QList<std::shared_ptr<CurveModel>> EditorModel::curves() const
{
    return m_curves;
}

QList<std::shared_ptr<StepCurveModel>> EditorModel::stepCurves() const
{
    return m_stepCurves;
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
    if (!addCurveInternal(curve, m_curves))
        return;

    emit curveAdded(curve);
}

void EditorModel::addStepCurve(std::shared_ptr<StepCurveModel> curve)
{
    qDebug() << "EditorModel::addStepCurve";

    if (!addCurveInternal(curve, m_stepCurves))
        return;

    emit stepCurveAdded(curve);
}

template<class T>
bool EditorModel::addCurveInternal(std::shared_ptr<T> curve, CurveContainer<T>& container)
{
    if (!curve)
    {
        qWarning() << "Trying to add bad curve";
        return false;
    }

    if (container.contains(curve))
    {
        qWarning() << "Trying to add duplicate curve:" << curve->name();
        return false;
    }

    container.push_back(curve);
    return true;
}

void EditorModel::removeCurve(std::shared_ptr<CurveModel> curve)
{
    if (!removeCurveInternal(curve, m_curves))
        return;

    emit curveRemoved(curve);
}

void EditorModel::removeStepCurve(std::shared_ptr<StepCurveModel> curve)
{
    if (!removeCurveInternal(curve, m_stepCurves))
        return;

    emit stepCurveRemoved(curve);
}

template<class T>
bool EditorModel::removeCurveInternal(std::shared_ptr<T> curve, CurveContainer<T>& container)
{
    if (!curve)
    {
        qWarning() << "Trying to remove bad curve";
        return false;
    }

    int removed = container.removeAll(curve);
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

void EditorModel::handleRequestToAddNewCurve(std::shared_ptr<CurveModel> curve)
{
    emit requestToAddNewCurve(curve);
}

void EditorModel::handleRequestToAddNewStepCurve(std::shared_ptr<StepCurveModel> curve)
{
    qDebug() << "EditorModel::handleRequestToAddNewStepCurve";
    emit requestToAddNewStepCurve(curve);
}

