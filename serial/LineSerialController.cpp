//
// Created by chang on 2018-07-22.
//

#include <global.h>
#include <QDebug>
#include "LineSerialController.h"

QByteArray LineSerialController::getNextFrame() {
    if (_dataList.isEmpty()) {
        return QByteArray();
    }

    if (_currentCount >= _dataList.count()) {
        _currentCount =0;
    }


    auto line = _dataList[_currentCount++];
    if (_isHex) {
        return dataFromHex(line);
    } else {
        return line.toLocal8Bit();
    }
}

MainWindow::SendType LineSerialController::sendType() {
    return MainWindow::SendType ::Line;
}

LineSerialController::LineSerialController(SerialController *serialController) : SerialController(serialController) {

}

LineSerialController::LineSerialController():SerialController() {

}

int LineSerialController::getTotalCount() {
    return _dataList.count();
}
