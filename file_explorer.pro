#-------------------------------------------------
#
# Project created by QtCreator 2014-08-06T02:50:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = file_explorer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    utils.cpp \
    renamedialog.cpp \
    copydialog.cpp

HEADERS  += mainwindow.h \
    utils.h \
    renamedialog.h \
    copydialog.h

FORMS    += mainwindow.ui \
    renamedialog.ui \
    copydialog.ui
