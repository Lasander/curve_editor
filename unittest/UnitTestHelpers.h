#ifndef UNITTESTHELPERS_H
#define UNITTESTHELPERS_H

#include <QtTest/QtTest>
#include <QDebug>

/**
 * @brief Output message handler to suppress any debug prints.
 *
 * Singleton that can used through start/stop methods or by creating an instance
 * which calls start on construction and stop on destruction.
 *
 * Scoped instance can be created using macro SUPPRESS_DEBUG_IN_SCOPE.
 */
class SuppressDebug
{
public:
    /** @brief SuppressDebug instance will automatically start/stop the suppressing. */
    SuppressDebug() { start(); }
    ~SuppressDebug() { stop(); }

    /** Start suppressing debug messages */
    static void start();
    /** Stop suppressing debug messages */
    static void stop();

    /**
     * @brief The Previous handler.
     * Public to allow easy access to the handler function. Shouldn't be used by clients of this class.
     */
    static QtMessageHandler s_prevHandler;

private:
    /** Flag to control the the handler is not installed multiple times (stacked) */
    static bool s_isStarted;
};

#define SUPPRESS_DEBUG_IN_SCOPE SuppressDebug suppressDebug##__LINE__;

/**
 * @brief Helper class to test that certain operation results in an error logging.
 *
 * The class is used through the EXPECT_ERRORS macro, which will create an instance
 * of this class (from stack). When that instance is destroyed (the current scope ends)
 * the error count will be checked against the creation moment.
 *
 * The actual logging is suppressed while errors are expected to keep the test outpu clean.
 */
class ExpectErrors
{
public:
    /**
     * @brief Construct ExpectErrors instance. Prefer not using directly but through the EXPECT_ERRORS macro.
     * @param file File name to be used is possible test fail logging
     * @param line Line number to be used is possible test fail logging
     */
    ExpectErrors(const char* file, int line);

    /** Destructor */
    ~ExpectErrors();

private:
    int m_startErrorCount;
    QtMessageHandler m_prevHandler;
    const char* m_file;
    int m_line;
};

#define EXPECT_ERRORS ExpectErrors expectErrorsStartingFromLine(__FILE__, __LINE__);

#endif // UNITTESTHELPERS_H
