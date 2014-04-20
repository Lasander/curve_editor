#-------------------------------------------------
#
# Project created by QtCreator 2014-04-20T10:24:11
#
#-------------------------------------------------

include(../curveeditor.pro)

QT += testlib

TARGET = curveeditor_tests
CONFIG   += console
CONFIG   -= app_bundle

# Search ".." for source not found otherwise
VPATH = ..

# Replace normal main() with test main()
SOURCES -= main.cpp
SOURCES += maintest.cpp \
    UnitTestHelpers.cpp

SOURCES += \
    Test_CurveModel.cpp \

HEADERS += \
    Test_CurveModel.h \
    CurveTestReceiver.h \
    UnitTestHelpers.h

