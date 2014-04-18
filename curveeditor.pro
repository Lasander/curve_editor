#-------------------------------------------------
#
# Project created by QtCreator 2014-04-13T21:37:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Editor
TEMPLATE = app

QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+

CONFIG += c++11

#QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    CurveModel.cpp \
    CurveView.cpp \
    EditorModel.cpp \
    EditorView.cpp \
    GraphicsItem.cpp \
    GraphicsItemDelegate.cpp \
    GraphicsItemEvent.cpp \
    main.cpp \
    MainWindow.cpp \
    PointItem.cpp \
    PointView.cpp \
    RangeF.cpp \
    ScaleView.cpp \
    ScrollPositionKeeper.cpp \
    TransformationNode.cpp \
    pt/math/cubic_hermite_spline.inl

HEADERS  += \
    CurveModel.h \
    CurveView.h \
    EditorModel.h \
    EditorView.h \
    GraphicsItem.h \
    GraphicsItemDelegate.h \
    GraphicsItemEvent.h \
    MainWindow.h \
    PointItem.h \
    PointView.h \
    RangeF.h \
    ScaleView.h \
    ScrollPositionKeeper.h \
    TransformationNode.h \
    pt/math/cubic_hermite_spline.h \
    pt/math/kb_data_set.h \
    pt/math/kb_spline.h
