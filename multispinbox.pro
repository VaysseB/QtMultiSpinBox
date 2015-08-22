#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T18:59:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = multispinbox
TEMPLATE = app

INCLUDEPATH += "/usr/include/qt5/QtCore/5.2.1/QtCore"
INCLUDEPATH += "/usr/include/qt5/QtGui/5.2.1/QtGui"
INCLUDEPATH += "/usr/include/qt5/QtWidgets/5.2.1/QtWidgets"

SOURCES += main.cpp\
    mainwindow.cpp \
    qtmultispinboxelements.cpp \
    qtmultispinbox.cpp

HEADERS  += \
    mainwindow.h \
    qtmultispinboxelements.h \
    qtmultispinbox.h \
    QtMultiSpinBox \
    QtMultiSpinBoxElements

FORMS += \
    mainwindow.ui
