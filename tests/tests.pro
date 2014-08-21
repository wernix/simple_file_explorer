#-------------------------------------------------
#
# Project created by QtCreator 2014-08-06T02:50:40
#
#-------------------------------------------------

include(../file_explorer.pri)
SOURCES -= main.cpp
VPATH += ..
INCLUDEPATH += ..

QT       += core testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = file_explorer_tests

CONFIG += testcase

TEMPLATE = app

SOURCES += \
    utilstest.cpp

HEADERS  += \
    utilstest.h
