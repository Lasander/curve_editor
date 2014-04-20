#include <QtTest/QtTest>

#include "Test_CurveModel.h"

int main()
{
    {
        Test_CurveModel test;
        QTest::qExec(&test);
    }

    return 0;
}
