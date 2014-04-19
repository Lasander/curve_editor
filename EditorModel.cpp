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

EditorModel::EditorModel()
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

void EditorModel::addCurve(std::shared_ptr<CurveModel> model)
{
    if (m_curves.contains(model))
        return;
    
    connect(model.get(), &CurveModel::timeRangeChanged, this, &EditorModel::curveTimeRangeChanged);
            
    m_curves.push_back(model);
    emit curveAdded(model);
    
    updateTimeRange();
}

void EditorModel::removeCurve(std::shared_ptr<CurveModel> model)
{
    int removed = m_curves.removeAll(model);
    if (removed < 1)
        return;

    disconnect(model.get(), &CurveModel::timeRangeChanged, this, &EditorModel::curveTimeRangeChanged);
    
    emit curveRemoved(model);
    
    updateTimeRange();
}

void EditorModel::clearCurves()
{
    for (auto curve : m_curves)
        removeCurve(curve);
}

void EditorModel::curveTimeRangeChanged(RangeF)
{
    updateTimeRange();
}

void EditorModel::setSceneTimeRange(RangeF timeRange)
{
    m_sceneTimeRange = timeRange;
    updateTimeRange();
}

void EditorModel::updateTimeRange()
{
    RangeF oldTimeRange = m_timeRange;
    m_timeRange = calculateTimeRange();
    
    if (m_timeRange != oldTimeRange)
    {
    	emit timeRangeChanged(m_timeRange);
    }
}

RangeF EditorModel::calculateTimeRange() const
{
    if (m_curves.size() == 0)
    	return RangeF();
    
    RangeF newTimeRange;
	for (auto c : m_curves)
        newTimeRange = RangeF::makeUnion(newTimeRange, c->timeRange());
    
    // Include also scene time range
    newTimeRange = RangeF::makeUnion(newTimeRange, m_sceneTimeRange);

    return newTimeRange;
}
