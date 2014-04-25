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
    UnitTestHelpers.cpp \
    Test_CurveModel.cpp \
    Test_SceneModel.cpp \
    Test_EditorModel.cpp

HEADERS += \
    UnitTestHelpers.h \
    Test_CurveModel.h \
    CurveTestReceiver.h \
    Test_SceneModel.h \
    SceneTestReceiver.h \
    Test_EditorModel.h \
    EditorTestReceiver.h

