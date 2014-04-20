#ifndef UNITTESTHELPERS_H
#define UNITTESTHELPERS_H

#include <QtTest/QtTest>
#include <QDebug>

void suppressOutput(QtMsgType, const QMessageLogContext&, const QString&);

class ExpectErrors
{
public:
    ExpectErrors();
    ~ExpectErrors();

private:
    static int s_errorCount;
    friend void suppressOutput(QtMsgType, const QMessageLogContext&, const QString&);

    int m_startErrorCount;
    QtMessageHandler m_prevHandler;
};

#endif // UNITTESTHELPERS_H
