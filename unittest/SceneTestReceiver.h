#ifndef SCENETESTRECEIVER_H
#define SCENETESTRECEIVER_H

#include <QObject>

#include "../RangeF.h"
#include "../CurveModel.h"
#include "../SceneModel.h"

class SceneTestReceiver : public QObject
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
    void curveSelected(std::shared_ptr<CurveModel> curve)
    {
        lastSelected = curve;
        ++selectedCount;
    }
    void curveDeselected(std::shared_ptr<CurveModel> curve)
    {
        lastDeselected = curve;
        ++deselectedCount;
    }
    void timeRangeChanged(RangeF newRange)
    {
        lastTimeRange = newRange;
        ++timeRangeChangeCount;
    }

public:
    std::shared_ptr<CurveModel> lastAdded;
    int addedCount;
    std::shared_ptr<CurveModel> lastRemoved;
    int removedCount;

    std::shared_ptr<CurveModel> lastSelected;
    int selectedCount;
    std::shared_ptr<CurveModel> lastDeselected;
    int deselectedCount;

    RangeF lastTimeRange;
    int timeRangeChangeCount;

    void reset()
    {
        lastAdded.reset();
        addedCount = 0;
        lastRemoved.reset();
        removedCount = 0;

        lastSelected.reset();
        selectedCount = 0;
        lastDeselected.reset();
        deselectedCount = 0;

        lastTimeRange = RangeF();
        timeRangeChangeCount = 0;
    }

    SceneTestReceiver(const SceneModel& scene)
    {
        reset();

        connect(&scene, &SceneModel::curveAdded, this, &SceneTestReceiver::curveAdded);
        connect(&scene, &SceneModel::curveRemoved, this, &SceneTestReceiver::curveRemoved);
        connect(&scene, &SceneModel::curveSelected, this, &SceneTestReceiver::curveSelected);
        connect(&scene, &SceneModel::curveDeselected, this, &SceneTestReceiver::curveDeselected);
        connect(&scene, &SceneModel::timeRangeChanged, this, &SceneTestReceiver::timeRangeChanged);
    }

    ~SceneTestReceiver()
    {
    }
};

#endif // SCENETESTRECEIVER_H
