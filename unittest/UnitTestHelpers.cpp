#include "UnitTestHelpers.h"

namespace {

/** Current error count */
int g_errorCount = 0;

/** Alternative message handler to suppress output, but record number of error logs. */
void suppressOutput(QtMsgType type, const QMessageLogContext&, const QString&)
{
    // Count anything logged at warning level or above as error
    if (type >= QtWarningMsg)
        ++g_errorCount;
}

} // anonymous namespace

ExpectErrors::ExpectErrors(const char* file, int line)
  : m_startErrorCount(g_errorCount),
    m_prevHandler(qInstallMessageHandler(suppressOutput)),
    m_file(file),
    m_line(line)
{
}

ExpectErrors::~ExpectErrors()
{
    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), "Expected errors, but got none at %s:%d", m_file, m_line);
    QVERIFY2(m_startErrorCount < g_errorCount, msgBuffer);
    qInstallMessageHandler(m_prevHandler);
}
