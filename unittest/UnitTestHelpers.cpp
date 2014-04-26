#include "UnitTestHelpers.h"

namespace {

/** Current error count */
int g_errorCount = 0;

/** Alternative message handler to suppress output, but record number of error logs. */
void suppressDebug(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // For debug messages do nothing
    if (type == QtDebugMsg)
        return;

    // Call previous handler for other message
    SuppressDebug::s_prevHandler(type, context, msg);
}

/** Alternative message handler to suppress output, but record number of error logs. */
void suppressOutput(QtMsgType type, const QMessageLogContext&, const QString&)
{
    // Count anything logged at warning level or above as error
    if (type >= QtWarningMsg)
        ++g_errorCount;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

QtMessageHandler SuppressDebug::s_prevHandler = nullptr;
bool SuppressDebug::s_isStarted = false;

void SuppressDebug::start()
{
    if (SuppressDebug::s_isStarted)
        return;

    SuppressDebug::s_isStarted = true;
    SuppressDebug::s_prevHandler = qInstallMessageHandler(suppressDebug);
}

void SuppressDebug::stop()
{
    if (!SuppressDebug::s_isStarted)
        return;

    qInstallMessageHandler(SuppressDebug::s_prevHandler);
    SuppressDebug::s_isStarted = false;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

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
