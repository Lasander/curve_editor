#ifndef SCENETESTRECEIVER_H
#define SCENETESTRECEIVER_H

#include <QObject>

#include "../RangeF.h"
#include "../CurveModel.h"
#include "../EditorModel.h"

class EditorTestReceiver : public QObject
{
    Q_OBJECT

public slots:
    void curveAdded(std::shared_ptr<CurveModel> curve)
    {
        lastAdded = curve;
        ++addedCount;
    }
    void curveRemoved(std::shared_ptr<CurveModel> curve)
    {
        lastRemoved = curve;
        ++removedCount;
    }
    void timeRangeChanged(RangeF newRange)
    {
        lastTimeRange = newRange;
        ++timeRangeChangeCount;
    }

    void beatOffsetChanged(double beatOffset)
    {
        lastBeatOffset = beatOffset;
        ++beatOffsetChangedCount;
    }
    void bpmChanged(double bpm)
    {
        lastBpm = bpm;
        ++bpmChangedCount;
    }

public:
    std::shared_ptr<CurveModel> lastAdded;
    int addedCount;
    std::shared_ptr<CurveModel> lastRemoved;
    int removedCount;

    RangeF lastTimeRange;
    int timeRangeChangeCount;

    double lastBeatOffset;
    int beatOffsetChangedCount;
    double lastBpm;
    int bpmChangedCount;

    void reset()
    {
        lastAdded.reset();
        addedCount = 0;
        lastRemoved.reset();
        removedCount = 0;

        lastTimeRange = RangeF();
        timeRangeChangeCount = 0;

        lastBeatOffset = 0.0;
        beatOffsetChangedCount = 0;
        lastBpm = 0.0;
        bpmChangedCount = 0;
    }

    EditorTestReceiver(const EditorModel& editor)
    {
        reset();

        connect(&editor, &EditorModel::curveAdded, this, &EditorTestReceiver::curveAdded);
        connect(&editor, &EditorModel::curveRemoved, this, &EditorTestReceiver::curveRemoved);
        connect(&editor, &EditorModel::timeRangeChanged, this, &EditorTestReceiver::timeRangeChanged);
        connect(&editor, &EditorModel::beatOffsetChanged, this, &EditorTestReceiver::beatOffsetChanged);
        connect(&editor, &EditorModel::bpmChanged, this, &EditorTestReceiver::bpmChanged);
    }

    ~EditorTestReceiver()
    {
    }
};

#endif // SCENETESTRECEIVER_H
