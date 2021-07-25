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
    _dataList.clear();
    for (auto &s :serialController->_dataList) {
        _dataList.append(s);
    }
    _currentCount = 0;
}

SerialController::~SerialController() {
}

int SerialController::getCurrentCount() {
    return _currentCount;
}

void SerialController::setCurrentCount(int count) {
    _currentCount = count;
}

void SerialController::setData(const QList<QByteArray> &dataList) {
    _dataList = dataList;
    _currentCount = 0;
}

bool SerialController::readEnd() {
    return _currentCount > _dataList.count() - 1;
}

int SerialController::getTotalCount() {
    return _dataList.count();
}

void SerialController::decreaseCurrentCount() {
    if (_currentCount > 0) {
        _currentCount--;
    }
}

void SerialController::increaseCurrentCount() {
    _currentCount++;
}


