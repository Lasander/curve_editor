#include "UnitTestHelpers.h"

int ExpectErrors::s_errorCount = 0;

void suppressOutput(QtMsgType type, const QMessageLogContext&, const QString&)
{
    // Count anything logged at warning level or above as error
    if (type >= QtWarningMsg)
        ++ExpectErrors::s_errorCount;
}

ExpectErrors::ExpectErrors()
  : m_startErrorCount(s_errorCount),
    m_prevHandler(qInstallMessageHandler(suppressOutput))
{
}

ExpectErrors::~ExpectErrors()
{
    QVERIFY2(m_startErrorCount < s_errorCount, "Expected errors, but got none");
    qInstallMessageHandler(m_prevHandler);
}
