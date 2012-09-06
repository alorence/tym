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
    searchprovider.cpp

HEADERS  += mainwindow.h \
    librarymodel.h \
    libraryview.h \
    bpdatabase.h \
    libraryelementview.h \
    searchprovider.h

FORMS    += mainwindow.ui \
    libraryelementview.ui
