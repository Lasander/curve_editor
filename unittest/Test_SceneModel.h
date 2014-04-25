#ifndef TEST_SCENEMODEL_H
#define TEST_SCENEMODEL_H

#include <QtTest/QtTest>

class Test_SceneModel : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testConstruction();
    void testCurveAddRemove();
    void testCurveSelection();
    void testTimeRange();
    void testBeat();
    void testStandardEditors();
};

#endif // TEST_SCENEMODEL_H
