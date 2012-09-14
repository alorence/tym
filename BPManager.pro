#-------------------------------------------------
#
# Project created by QtCreator 2012-08-29T10:38:04
#
#-------------------------------------------------

QT       += core gui sql network
CONFIG   += debug

TARGET = BPManager
TEMPLATE = app

include(src/src.pri)
include(uis/uis.pri)

SOURCES += libs/qt-json/json.cpp

HEADERS  += libs/qt-json/json.h

RESOURCES += resources.qrc

