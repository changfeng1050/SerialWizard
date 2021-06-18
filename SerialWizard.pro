#-------------------------------------------------
#
# Project created by QtCreator 2017-08-03T09:01:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
                                QT += serialport
                                QT += network

CONFIG += c++11

TARGET = SerialWizard
TEMPLATE = app


SOURCES += main.cpp\
    DataProcessDialog.cpp \
    FrameInfoDialog.cpp \
    data/SerialBridgeReadWriter.cpp \
        mainwindow.cpp \
    global.cpp \
    CalculateCheckSumDialog.cpp \
    data/AbstractReadWriter.cpp \
    data/BridgeReadWriter.cpp \
    data/SerialReadWriter.cpp \
    data/TcpServerReadWriter.cpp \
    ConvertDataDialog.cpp \
    serial/LineSerialController.cpp \
    serial/SerialController.cpp \
    data/TcpClientReadWriter.cpp \
    utils/FileUtil.cpp

HEADERS  += mainwindow.h \
    DataProcessDialog.h \
    FrameInfoDialog.h \
    data/SerialBridgeReadWriter.h \
    global.h \
    CalculateCheckSumDialog.h \
    data/AbstractReadWriter.h \
    data/BridgeReadWriter.h \
    data/SerialReadWriter.h \
    data/TcpServerReadWriter.h \
    ConvertDataDialog.h \
    serial/LineSerialController.h \
    serial/SerialController.h \
    data/TcpClientReadWriter.h \
    utils/FileUtil.h
