#-------------------------------------------------
#
# Project created by QtCreator 2012-08-29T10:38:04
#
#-------------------------------------------------

QT       += core gui sql network

TARGET = BPManager
TEMPLATE = app

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/librarymodel.cpp \
    uis/libraryview.cpp \
    src/bpdatabase.cpp \
    uis/libraryelementview.cpp \
    src/searchprovider.cpp \
    uis/settingsdialog.cpp \
    libs/qt-json/json.cpp

HEADERS  += src/mainwindow.h \
    src/librarymodel.h \
    uis/libraryview.h \
    src/bpdatabase.h \
    uis/libraryelementview.h \
    src/searchprovider.h \
    uis/settingsdialog.h \
    libs/qt-json/json.h

UI_HEADERS_DIR += uis

FORMS    += uis/mainwindow.ui \
    uis/libraryelementview.ui \
    uis/settingsdialog.ui

RESOURCES += \
    res/resources.qrc
