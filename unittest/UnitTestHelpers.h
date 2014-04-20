#ifndef UNITTESTHELPERS_H
#define UNITTESTHELPERS_H

#include <QtTest/QtTest>
#include <QDebug>

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
