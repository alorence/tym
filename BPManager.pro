#-------------------------------------------------
#
# Project created by QtCreator 2012-08-29T10:38:04
#
#-------------------------------------------------

QT       += core gui sql network

TARGET = BPManager
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    librarymodel.cpp \
    libraryview.cpp \
    bpdatabase.cpp \
    libraryelementview.cpp \
    searchprovider.cpp \
    settingsdialog.cpp \
    libs/qt-json/json.cpp

HEADERS  += mainwindow.h \
    librarymodel.h \
    libraryview.h \
    bpdatabase.h \
    libraryelementview.h \
    searchprovider.h \
    settingsdialog.h \
    libs/qt-json/json.h

FORMS    += mainwindow.ui \
    libraryelementview.ui \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
