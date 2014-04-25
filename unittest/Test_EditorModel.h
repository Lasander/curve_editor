#ifndef TEST_EDITORMODEL_H
#define TEST_EDITORMODEL_H

#include <QtTest/QtTest>

class Test_EditorModel : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testConstruction();
    void testCurveAddRemove();
    void testTimeRange();
    void testBeat();
};

#endif // TEST_EDITORMODEL_H
