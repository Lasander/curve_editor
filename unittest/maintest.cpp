#include <QtTest/QtTest>

#include "Test_CurveModel.h"
#include "Test_SceneModel.h"
#include "Test_EditorModel.h"

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
    {
        Test_EditorModel test;
        QTest::qExec(&test);
    }

    return 0;
}
