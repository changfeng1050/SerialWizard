//
// Created by chang on 2018-07-22.
//

#include <global.h>
#include <QDebug>
#include "LineSerialController.h"

QByteArray LineSerialController::readNextFrame() {
    if (_dataList.isEmpty()) {
        return QByteArray();
    }

    if (_currentCount >= _dataList.count()) {
        _currentCount = 0;
    }

    auto line = _dataList[_currentCount++];
    return line;
}

MainWindow::SendType LineSerialController::sendType() {
    return MainWindow::SendType::Line;
}

LineSerialController::LineSerialController(SerialController *serialController) : SerialController(serialController) {
}

LineSerialController::LineSerialController() : SerialController() {
}
