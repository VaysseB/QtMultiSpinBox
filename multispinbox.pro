#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T18:59:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = multispinbox
TEMPLATE = app

!include(multispinbox.pri) {
    error("Missing multispinbox.pri")
}

SOURCES += main.cpp\
    mainwindow.cpp

HEADERS  += \
    mainwindow.h

FORMS += \
    mainwindow.ui

OTHER_FILES += \
    multispinbox.pri
