#include "Test_EditorModel.h"

#include "../EditorModel.h"
#include "../CurveModel.h"
#include "EditorTestReceiver.h"
#include "UnitTestHelpers.h"

void Test_EditorModel::init()
{
}

void Test_EditorModel::cleanup()
{
}

void Test_EditorModel::testConstruction()
{
    RangeF timeRange(-10, 10);
    double beatOffset = 0.4;
    double bpm = 55.5;

    EditorModel model(timeRange, beatOffset, bpm);

    QVERIFY(model.curves().isEmpty());
    QCOMPARE(model.timeRange(), timeRange);
    QCOMPARE(model.beatOffset(), beatOffset);
    QCOMPARE(model.bpm(), bpm);
}

void Test_EditorModel::testCurveAddRemove()
{
    std::shared_ptr<CurveModel> nullCurve;

    { // Simple adding and removing
        EditorModel model(RangeF(10, 20), 0.0, 80.0);
        EditorTestReceiver receiver(model);

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
        EditorModel model(RangeF(10, 20), 0.0, 80.0);
        EditorTestReceiver receiver(model);

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
        EditorModel model(RangeF(10, 20), 0.0, 80.0);
        EditorTestReceiver receiver(model);

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

void Test_EditorModel::testTimeRange()
{
    EditorModel model(RangeF(), 0.0, 80.0);
    EditorTestReceiver receiver(model);

    // Initially invalid time range
    QVERIFY(!model.timeRange().isValid());

    // Set a valid time range
    model.setTimeRange(RangeF(10, 20));
    QCOMPARE(model.timeRange(), RangeF(10, 20));
    QCOMPARE(receiver.timeRangeChangeCount, 1);
    QCOMPARE(receiver.lastTimeRange, model.timeRange());

    // Set new valid time range
    model.setTimeRange(RangeF(15, 30));
    QCOMPARE(model.timeRange(), RangeF(15, 30));
    QCOMPARE(receiver.timeRangeChangeCount, 2);
    QCOMPARE(receiver.lastTimeRange, model.timeRange());

    // Set an invalid time range
    model.setTimeRange(RangeF());
    QVERIFY(!model.timeRange().isValid());
    QCOMPARE(receiver.timeRangeChangeCount, 3);
    QCOMPARE(receiver.lastTimeRange, model.timeRange());
}

void Test_EditorModel::testBeat()
{
    EditorModel model(RangeF(), 0.0, 80.0);
    EditorTestReceiver receiver(model);

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
