#include "Test_CurveModel.h"

#include "../CurveModel.h"
#include "CurveTestReceiver.h"
#include "UnitTestHelpers.h"

#include <QDebug>

void Test_CurveModel::init()
{
}

void Test_CurveModel::cleanup()
{
}

void Test_CurveModel::testConstruction()
{
    { // Curve with name "Name"
        CurveModel curve("Name");
        CurveTestReceiver receiver(curve);

        QVERIFY(curve.pointIds().isEmpty());
        QCOMPARE(curve.numberOfPoints(), 0);

        QVERIFY(!curve.timeRange().isValid());
        QVERIFY(curve.valueRange().isValid());

        QCOMPARE(curve.isSelected(), false);

        QCOMPARE(curve.name(), QString("Name"));

        // No notifications from the curve
        QCOMPARE(receiver.addedCount, 0);
        QCOMPARE(receiver.updatedCount, 0);
        QCOMPARE(receiver.removedCount, 0);
    }
}

void Test_CurveModel::testPointAddUpdateRemove()
{
    { // Test simple adding, updating and removing
        CurveModel curve("Name");
        CurveTestReceiver receiver(curve);

        // Add point (1, 10)
        curve.addPoint(1, 10);
        QCOMPARE(curve.numberOfPoints(), 1);
        QCOMPARE(receiver.addedCount, 1);
        PointId first = receiver.lastAdded;
        QVERIFY(curve.pointIds().contains(first));

        // Add point (5, -1)
        curve.addPoint(5, -1);
        QCOMPARE(curve.numberOfPoints(), 2);
        QCOMPARE(receiver.addedCount, 2);
        PointId second = receiver.lastAdded;
        QVERIFY(curve.pointIds().contains(first));
        QVERIFY(curve.pointIds().contains(second));

        // Update first point (1, 10) => (7, 2)
        curve.updatePoint(first, 7, 2);
        QCOMPARE(curve.numberOfPoints(), 2);
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.updatedCount, 1);
        QCOMPARE(receiver.removedCount, 0);
        QCOMPARE(receiver.lastUpdated, first);
        QVERIFY(curve.pointIds().contains(first));
        QVERIFY(curve.pointIds().contains(second));

        // Remove second point
        curve.removePoint(second);
        QCOMPARE(curve.numberOfPoints(), 1);
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.updatedCount, 1);
        QCOMPARE(receiver.removedCount, 1);
        QCOMPARE(receiver.lastRemoved, second);
        QVERIFY(curve.pointIds().contains(first));
        QVERIFY(!curve.pointIds().contains(second));

        // Remove first point
        curve.removePoint(first);
        QCOMPARE(curve.numberOfPoints(), 0);
        QCOMPARE(receiver.addedCount, 2);
        QCOMPARE(receiver.updatedCount, 1);
        QCOMPARE(receiver.removedCount, 2);
        QCOMPARE(receiver.lastRemoved, first);
        QVERIFY(!curve.pointIds().contains(first));
        QVERIFY(!curve.pointIds().contains(second));
    }

    { // Test update errors
        CurveModel curve("Name");
        CurveTestReceiver receiver(curve);

        // Add point (1, 4)
        curve.addPoint(1, 4);
        QCOMPARE(receiver.addedCount, 1);
        PointId first = receiver.lastAdded;

        // Try to update point with another id
        {
            EXPECT_ERRORS;
            curve.updatePoint(PointId::generateId(), 2, 5);
        }

        // Try to update point with invalid id
        {
            EXPECT_ERRORS;
            curve.updatePoint(PointId::invalidId(), 2, 5);
        }
        QCOMPARE(receiver.addedCount, 1);
        QCOMPARE(receiver.updatedCount, 0);

        // Update point successfully
        curve.updatePoint(first, 2, 5);
        QCOMPARE(receiver.updatedCount, 1);
    }

    { // Test remove errors
        CurveModel curve("Name");
        CurveTestReceiver receiver(curve);

        // Add point (1, 4)
        curve.addPoint(1, 4);
        QCOMPARE(receiver.addedCount, 1);

        // Try to remove point with another id
        {
            EXPECT_ERRORS;
            curve.removePoint(PointId::generateId());
        }

        // Try to update point with invalid id
        {
            EXPECT_ERRORS;
            curve.removePoint(PointId::invalidId());
        }
        QCOMPARE(receiver.addedCount, 1);
        QCOMPARE(receiver.removedCount, 0);
    }
}

void Test_CurveModel::testSelection()
{
    CurveModel curve("Name");
    CurveTestReceiver receiver(curve);

    // Initially deselected
    QCOMPARE(curve.isSelected(), false);

    // Deselecting again won't do anything
    curve.setSelected(false);
    QCOMPARE(curve.isSelected(), false);
    QCOMPARE(receiver.selectionChangeCount, 0);

    // Select
    curve.setSelected(true);
    QCOMPARE(curve.isSelected(), true);
    QCOMPARE(receiver.selectionChangeCount, 1);
    QCOMPARE(receiver.lastSelectionStatus, curve.isSelected());

    // Select again (no change)
    curve.setSelected(true);
    QCOMPARE(curve.isSelected(), true);
    QCOMPARE(receiver.selectionChangeCount, 1);

    // Deselect
    curve.setSelected(false);
    QCOMPARE(curve.isSelected(), false);
    QCOMPARE(receiver.selectionChangeCount, 2);
    QCOMPARE(receiver.lastSelectionStatus, curve.isSelected());
}
