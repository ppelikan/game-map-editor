#-------------------------------------------------
#
# Project created by QtCreator 2016-02-16T00:25:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapEditor
TEMPLATE = app

TRANSLATIONS += \
    translations/MapEditor_en.ts \
    translations/MapEditor_pl.ts \

SOURCES += main.cpp\
        mainwindow.cpp \
    tilebuffer.cpp \
    tilemap.cpp \
    tiledmapeditor.cpp \
    tileselector.cpp \
    animationbuffer.cpp \
    animationeditor.cpp \
    neweventdialog.cpp \
    eventmatrix.cpp \
    entirelevel.cpp \
    levelundostack.cpp

HEADERS  += mainwindow.h \
    tilebuffer.h \
    tilemap.h \
    tiledmapeditor.h \
    tileselector.h \
    animationbuffer.h \
    animationeditor.h \
    neweventdialog.h \
    eventmatrix.h \
    entirelevel.h \
    levelundostack.h

FORMS    += mainwindow.ui \
    neweventdialog.ui

RESOURCES += \
    menuicons.qrc

RC_FILE = mapeditor.rc

DISTFILES += \
    mapeditor.rc
