#ifndef TEST_CURVEMODEL_H
#define TEST_CURVEMODEL_H

#include <QtTest/QtTest>

class Test_CurveModel : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testContruction();
    void testPointAddUpdateRemove();
    void testSelection();
};

#endif // TEST_CURVEMODEL_H
