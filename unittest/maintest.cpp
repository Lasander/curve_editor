#include <QtTest/QtTest>

#include "Test_CurveModel.h"
#include "Test_SceneModel.h"

int main()
{
    {
        Test_CurveModel test;
        QTest::qExec(&test);
    }
    {
        Test_SceneModel test;
        QTest::qExec(&test);
    }

    return 0;
}
