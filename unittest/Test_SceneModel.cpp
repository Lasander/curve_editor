#include "Test_SceneModel.h"

#include "../SceneModel.h"
#include "../CurveModel.h"
#include "../EditorModel.h"
#include "SceneTestReceiver.h"
#include "UnitTestHelpers.h"

void Test_SceneModel::init()
{
}

void Test_SceneModel::cleanup()
{
}

void Test_SceneModel::testConstruction()
{
    SceneModel model;
    QVERIFY(model.curves().isEmpty());
    QVERIFY(!model.timeRange().isValid());

    QVERIFY(model.getAllCurvesEditor().get());
    QVERIFY(model.getSelectedCurvesEditor().get());
}

void Test_SceneModel::testCurveAddRemove()
{
    std::shared_ptr<CurveModel> nullCurve;

    { // Simple adding and removing
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");

        model.addCurve(curve1);
        QVERIFY(model.curves().contains(curve1));
        QCOMPARE(receiver.addedCount, 1);
        QCOMPARE(receiver.lastAdded, curve1);
        QCOMPARE(receiver.removedCount, 0);

        auto curve2 = std::make_shared<CurveModel>(1, "Second");

        model.addCurve(curve2);
        QVERIFY(model.curves().contains(curve1));
        QVERIFY(model.curves().contains(curve2));
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.lastAdded, curve2);
        QCOMPARE(receiver.removedCount, 0);

        model.removeCurve(curve1);
        QVERIFY(!model.curves().contains(curve1));
        QVERIFY(model.curves().contains(curve2));
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.removedCount, 1);
        QCOMPARE(receiver.lastRemoved, curve1);

        model.removeCurve(curve2);
        QVERIFY(model.curves().isEmpty());
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.removedCount, 2);
        QCOMPARE(receiver.lastRemoved, curve2);
    }

    { // Test add error cases
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");

        model.addCurve(curve1);
        QVERIFY(model.curves().contains(curve1));
        QCOMPARE(receiver.addedCount, 1);
        QCOMPARE(receiver.removedCount, 0);

        // Add duplicate, no effects
        {
            EXPECT_ERRORS;
            model.addCurve(curve1);
        }
        QVERIFY(model.curves().contains(curve1));
        QCOMPARE(receiver.addedCount, 1);
        QCOMPARE(receiver.removedCount, 0);

        // Add null curve, no effects
        {
            EXPECT_ERRORS;
            model.addCurve(nullCurve);
        }
        QVERIFY(model.curves().contains(curve1));
        QCOMPARE(receiver.addedCount, 1);
        QCOMPARE(receiver.removedCount, 0);
    }

    { // Test remove error cases
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");
        auto curve2 = std::make_shared<CurveModel>(1, "Second");
        auto curve3 = std::make_shared<CurveModel>(1, "Third");

        // Remove from empty, no effect
        {
            EXPECT_ERRORS;
            model.removeCurve(curve1);
        }
        QCOMPARE(receiver.addedCount, 0);
        QCOMPARE(receiver.removedCount, 0);

        // Add two curves
        model.addCurve(curve1);
        model.addCurve(curve2);
        QVERIFY(model.curves().contains(curve1));
        QVERIFY(model.curves().contains(curve2));
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.removedCount, 0);

        // Remove curve that hasn't been added, no effect
        {
            EXPECT_ERRORS;
            model.removeCurve(nullCurve);
        }
        QVERIFY(model.curves().contains(curve1));
        QVERIFY(model.curves().contains(curve2));
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.removedCount, 0);

        // Remove successfully
        model.removeCurve(curve2);
        QVERIFY(model.curves().contains(curve1));
        QVERIFY(!model.curves().contains(curve2));
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.removedCount, 1);

        // Duplicate remove, no effect
        {
            EXPECT_ERRORS;
            model.removeCurve(curve2);
        }
        QVERIFY(model.curves().contains(curve1));
        QVERIFY(!model.curves().contains(curve2));
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.removedCount, 1);
    }
}

void Test_SceneModel::testCurveSelection()
{
    std::shared_ptr<CurveModel> nullCurve;

    { // Test simple selection/deselection
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");
        auto curve2 = std::make_shared<CurveModel>(1, "Second");

        // Add curves
        model.addCurve(curve1);
        model.addCurve(curve2);

        // Select one
        model.selectCurve(curve2);
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.lastSelected, curve2);
        QCOMPARE(receiver.deselectedCount, 0);

        // Select another
        model.selectCurve(curve1);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.lastSelected, curve1);
        QCOMPARE(receiver.deselectedCount, 0);

        // Deselect
        model.deselectCurve(curve2);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.deselectedCount, 1);
        QCOMPARE(receiver.lastDeselected, curve2);

        // Deselect another
        model.deselectCurve(curve1);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.deselectedCount, 2);
        QCOMPARE(receiver.lastDeselected, curve1);
    }

    { // Test selection/deselection outside of the scene
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");
        auto curve2 = std::make_shared<CurveModel>(1, "Second");
        auto curve3 = std::make_shared<CurveModel>(1, "Third");

        // Add (unselected) curves
        model.addCurve(curve1);
        model.addCurve(curve2);

        // Select one
        curve2->setSelected(true);
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.lastSelected, curve2);
        QCOMPARE(receiver.deselectedCount, 0);

        // Select another
        curve1->setSelected(true);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.lastSelected, curve1);
        QCOMPARE(receiver.deselectedCount, 0);

        // Deselect
        curve2->setSelected(false);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.deselectedCount, 1);
        QCOMPARE(receiver.lastDeselected, curve2);

        // Remove selected curve (will deselect)
        model.removeCurve(curve1);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.deselectedCount, 2);
        QCOMPARE(receiver.lastDeselected, curve1);
        QVERIFY(!curve1->isSelected());

        // Remove deselected curve
        model.removeCurve(curve2);
        QCOMPARE(receiver.selectedCount, 2);
        QCOMPARE(receiver.deselectedCount, 2);

        // Manually select curve
        curve1->setSelected(true);

        // Add selected curve (should be notified)
        model.addCurve(curve1);
        QCOMPARE(receiver.selectedCount, 3);
        QCOMPARE(receiver.lastSelected, curve1);
        QCOMPARE(receiver.deselectedCount, 2);
    }

    { // Test selection errors
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");
        auto curve2 = std::make_shared<CurveModel>(1, "Second");
        auto curve3 = std::make_shared<CurveModel>(1, "Third");

        // Add one curve and select
        model.addCurve(curve1);
        model.selectCurve(curve1);
        QCOMPARE(receiver.selectedCount, 1);

        // Select null curve, no effect
        {
            EXPECT_ERRORS;
            model.selectCurve(nullCurve);
        }
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 0);

        // Select curve that hasn't been added to the scene, no effect
        {
            EXPECT_ERRORS;
            model.selectCurve(curve2);
        }
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 0);

        // Select curve that is already selected, no effect
        {
            EXPECT_ERRORS;
            model.selectCurve(curve1);
        }
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 0);
    }

    { // Test deselection errors
        SceneModel model;
        SceneTestReceiver receiver(model);

        auto curve1 = std::make_shared<CurveModel>(1, "First");
        auto curve2 = std::make_shared<CurveModel>(1, "Second");

        // Add one curve, select and deselect
        model.addCurve(curve1);
        model.selectCurve(curve1);
        model.deselectCurve(curve1);
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 1);

        // Deselect null curve, no effect
        {
            EXPECT_ERRORS;
            model.deselectCurve(nullCurve);
        }
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 1);

        // Deselect curve that hasn't been added to the scene, no effect
        {
            EXPECT_ERRORS;
            model.deselectCurve(curve2);
        }
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 1);

        // Select curve that is already deselected, no effect
        {
            EXPECT_ERRORS;
            model.deselectCurve(curve1);
        }
        QCOMPARE(receiver.selectedCount, 1);
        QCOMPARE(receiver.deselectedCount, 1);
    }
}

void Test_SceneModel::testTimeRange()
{
    SceneModel model;
    SceneTestReceiver receiver(model);

    // Initially invalid time range
    QVERIFY(!model.timeRange().isValid());

    auto curve1 = std::make_shared<CurveModel>(1, "First");
    auto curve2 = std::make_shared<CurveModel>(1, "Second");

    // Added curve should have matching time range
    model.addCurve(curve1);
    QCOMPARE(model.timeRange(), curve1->timeRange());
    QCOMPARE(receiver.timeRangeChangeCount, 0);

    // Set a valid time range
    model.setTimeRange(RangeF(10, 20));
    QCOMPARE(model.timeRange(), RangeF(10, 20));
    QCOMPARE(receiver.timeRangeChangeCount, 1);
    QCOMPARE(receiver.lastTimeRange, model.timeRange());
    QCOMPARE(model.timeRange(), curve1->timeRange()); // Note: This synchronization is responsibility of CurveModel

    // Added curve should have matching time range
    model.addCurve(curve2);
    QCOMPARE(model.timeRange(), curve2->timeRange());

    // Set new valid time range
    model.setTimeRange(RangeF(15, 30));
    QCOMPARE(model.timeRange(), RangeF(15, 30));
    QCOMPARE(receiver.timeRangeChangeCount, 2);
    QCOMPARE(receiver.lastTimeRange, model.timeRange());
    QCOMPARE(model.timeRange(), curve1->timeRange()); // Note: This synchronization is responsibility of CurveModel
    QCOMPARE(model.timeRange(), curve2->timeRange()); // Note: This synchronization is responsibility of CurveModel

    // Set an invalid time range
    model.setTimeRange(RangeF());
    QVERIFY(!model.timeRange().isValid());
    QCOMPARE(receiver.timeRangeChangeCount, 3);
    QCOMPARE(receiver.lastTimeRange, model.timeRange());
    QCOMPARE(model.timeRange(), curve1->timeRange()); // Note: This synchronization is responsibility of CurveModel
    QCOMPARE(model.timeRange(), curve2->timeRange()); // Note: This synchronization is responsibility of CurveModel
}

void Test_SceneModel::testBeat()
{
    SceneModel model;
    SceneTestReceiver receiver(model);

    // Initially default values
    QCOMPARE(model.beatOffset(), 0.0);
    QCOMPARE(model.bpm(), 80.0);
    QCOMPARE(receiver.beatOffsetChangedCount, 0);
    QCOMPARE(receiver.bpmChangedCount, 0);

    // Set a valid offset
    model.setBeatOffset(10.7);
    QCOMPARE(model.beatOffset(), 10.7);
    QCOMPARE(receiver.beatOffsetChangedCount, 1);
    QCOMPARE(receiver.bpmChangedCount, 0);
    QCOMPARE(receiver.lastBeatOffset, model.beatOffset());

    // Set a valid bpm
    model.setBpm(88.8);
    QCOMPARE(model.bpm(), 88.8);
    QCOMPARE(receiver.beatOffsetChangedCount, 1);
    QCOMPARE(receiver.bpmChangedCount, 1);
    QCOMPARE(receiver.lastBpm, model.bpm());

    // Set another valid values
    model.setBeatOffset(-1.5);
    model.setBpm(60.2);
    QCOMPARE(model.beatOffset(), -1.5);
    QCOMPARE(model.bpm(), 60.2);
    QCOMPARE(receiver.beatOffsetChangedCount, 2);
    QCOMPARE(receiver.bpmChangedCount, 2);
    QCOMPARE(receiver.lastBeatOffset, model.beatOffset());
    QCOMPARE(receiver.lastBpm, model.bpm());
}

void Test_SceneModel::testStandardEditors()
{
    SceneModel model;
    std::shared_ptr<EditorModel> allCurves = model.getAllCurvesEditor();
    std::shared_ptr<EditorModel> selectedCurves = model.getSelectedCurvesEditor();

    // No curves in scene, editors are empty
    QVERIFY(allCurves->curves().isEmpty());
    QVERIFY(selectedCurves->curves().isEmpty());

    auto curve1 = std::make_shared<CurveModel>(1, "First");
    auto curve2 = std::make_shared<CurveModel>(1, "Second");

    // Add (unselected) curve
    model.addCurve(curve1);
    QVERIFY(allCurves->curves().contains(curve1));
    QVERIFY(selectedCurves->curves().isEmpty());

    // Select the curve
    model.selectCurve(curve1);
    QVERIFY(allCurves->curves().contains(curve1));
    QVERIFY(selectedCurves->curves().contains(curve1));

    // Remove the curve (will also automatically deselect)
    model.removeCurve(curve1);
    QVERIFY(allCurves->curves().isEmpty());
    QVERIFY(selectedCurves->curves().isEmpty());

    // Manuall select the curve
    curve1->setSelected(true);

    // Add back (the now selected) curve
    model.addCurve(curve1);
    QVERIFY(allCurves->curves().contains(curve1));
    QVERIFY(selectedCurves->curves().contains(curve1));

    // Add another (unselected) curve
    model.addCurve(curve2);
    QVERIFY(allCurves->curves().contains(curve1));
    QVERIFY(allCurves->curves().contains(curve2));
    QVERIFY(selectedCurves->curves().contains(curve1));
    QVERIFY(!selectedCurves->curves().contains(curve2));

    // Select/deselect curves
    model.selectCurve(curve2);
    model.deselectCurve(curve1);
    QVERIFY(allCurves->curves().contains(curve1));
    QVERIFY(allCurves->curves().contains(curve2));
    QVERIFY(!selectedCurves->curves().contains(curve1));
    QVERIFY(selectedCurves->curves().contains(curve2));

    // Remove the curves
    model.removeCurve(curve1);
    model.removeCurve(curve2);
    QVERIFY(allCurves->curves().isEmpty());
    QVERIFY(selectedCurves->curves().isEmpty());
}
