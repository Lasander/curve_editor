//
//  CurveTestReceiver.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#ifndef CURVETESTRECEIVER_H
#define CURVETESTRECEIVER_H

#include "../CurveModel.h"
#include <QObject>

class CurveTestReceiver : public QObject
{
	Q_OBJECT
    
public slots:
    void pointAdded(PointId id)
    {
        lastAdded = id;
        ++addedCount;
    }
    
    void pointUpdated(PointId id)
    {
        lastUpdated = id;
        ++updatedCount;
    }
    
    void pointRemoved(PointId id)
    {
        lastRemoved = id;
        ++removedCount;
    }

    void timeRangeChanged(RangeF newRange)
    {
        lastTimeRange = newRange;
        ++timeRangeChangeCount;
    }

    void valueRangeChanged(RangeF newRange)
    {
        lastValueRange = newRange;
        ++valueRangeChangeCount;
    }

    void selectedChanged(bool status)
    {
        lastSelectionStatus = status;
        ++selectionChangeCount;
    }
    
public:
    PointId lastAdded;
    int addedCount;
	PointId lastUpdated;
    int updatedCount;
	PointId lastRemoved;
    int removedCount;
    
    RangeF lastTimeRange;
    int timeRangeChangeCount;
    RangeF lastValueRange;
    int valueRangeChangeCount;

    bool lastSelectionStatus;
    int selectionChangeCount;

    void reset()
    {
        lastAdded = PointId();
        addedCount = 0;
        lastUpdated = PointId();
        updatedCount = 0;
        lastRemoved = PointId();
        removedCount = 0;

        lastTimeRange = RangeF();
        timeRangeChangeCount = 0;
        lastValueRange = RangeF();
        valueRangeChangeCount = 0;

        lastSelectionStatus = false;
        selectionChangeCount = 0;
    }

    CurveTestReceiver(CurveModel const& curve)
    {
        reset();

    	connect(&curve, &CurveModel::pointAdded, this, &CurveTestReceiver::pointAdded);
    	connect(&curve, &CurveModel::pointUpdated, this, &CurveTestReceiver::pointUpdated);
        connect(&curve, &CurveModel::pointRemoved, this, &CurveTestReceiver::pointRemoved);
        connect(&curve, &CurveModel::selectedChanged, this, &CurveTestReceiver::selectedChanged);
    }
    
    ~CurveTestReceiver()
    {
    }
};

#endif /* CURVETESTRECEIVER_H */
