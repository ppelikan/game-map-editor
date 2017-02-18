#-------------------------------------------------
#
# Project created by QtCreator 2016-02-16T00:25:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tilebuffer.cpp \
    tilemap.cpp \
    tiledmapeditor.cpp \
    tileselector.cpp \
    animationbuffer.cpp \
    animationeditor.cpp \
    tiledselectablewidget.cpp

HEADERS  += mainwindow.h \
    tilebuffer.h \
    tilemap.h \
    tiledmapeditor.h \
    tileselector.h \
    animationbuffer.h \
    animationeditor.h \
    tiledselectablewidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    menuicons.qrc
