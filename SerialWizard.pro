#-------------------------------------------------
#
# Project created by QtCreator 2017-08-03T09:01:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

                                QT += serialport

CONFIG += c++11

TARGET = SerialWizard
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialport.cpp \
    global.cpp \
    FrameInfoDialog.cpp \
    CalculateCheckSumDialog.cpp

HEADERS  += mainwindow.h \
    serialport.h \
    global.h \
    FrameInfoDialog.h \
    CalculateCheckSumDialog.h
