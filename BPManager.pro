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
    src/libraryview.cpp \
    src/bpdatabase.cpp \
    src/libraryelementview.cpp \
    src/searchprovider.cpp \
    src/settingsdialog.cpp \
    libs/qt-json/json.cpp

HEADERS  += src/mainwindow.h \
    src/librarymodel.h \
    src/libraryview.h \
    src/bpdatabase.h \
    src/libraryelementview.h \
    src/searchprovider.h \
    src/settingsdialog.h \
    src/libs/qt-json/json.h

FORMS    += src/ui/mainwindow.ui \
    src/ui/libraryelementview.ui \
    src/ui/settingsdialog.ui

RESOURCES += \
    src/resources/resources.qrc
