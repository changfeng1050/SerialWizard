//
// Created by chang on 2018-07-22.
//

#include <global.h>
#include <QtCore/QTextStream>
#include "SerialController.h"
#include <QDebug>

SerialController::SerialController() {

}

SerialController::SerialController(SerialController *serialController) {
    _isHex = serialController->_isHex;
    _isAutoSend = serialController->_isAutoSend;
    _isLoopSend = serialController->_isLoopSend;
    _autoSendInterval = serialController->_autoSendInterval;

    _data = serialController->_data;
    _dataList.clear();
    for (auto s :serialController->_dataList) {
        _dataList.append(s);
    }
    _currentCount = 0;
}

SerialController::~SerialController() {

}

void SerialController::setData(QString data) {
    if (_isHex) {
        _data = dataFromHex(data);
    } else {
        _data = data.toLocal8Bit();
    }

    QTextStream in(&data);
    _dataList.clear();

    while (!in.atEnd()) {
        _dataList << in.readLine();
    }
}

void SerialController::setIsHex(bool isHex) {
    _isHex = isHex;
}

int SerialController::getCurrentCount() {
    return _currentCount;
}

void SerialController::setCurrentCount(int count) {
    _currentCount = count;
}

void SerialController::setAutoSend(bool autoSend) {
    _isAutoSend = autoSend;
}

void SerialController::setLoopSend(bool loopSend) {
    _isLoopSend = loopSend;
}

void SerialController::setAutoSendInterval(int interval) {
    _autoSendInterval = interval;
}

bool SerialController::isHex() {
    return _isHex;
}

bool SerialController::autoSend() {
    return _isAutoSend;
}

bool SerialController::loopSend() {
    return _isLoopSend;
}

int SerialController::autoSendInterval() {
    return _autoSendInterval;
}

QByteArray SerialController::data() {
    return _data;
}

QStringList SerialController::dataList() {
    return _dataList;
}
