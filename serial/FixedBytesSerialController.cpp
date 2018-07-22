//
// Created by chang on 2018-07-22.
//

#include "FixedBytesSerialController.h"

QByteArray FixedBytesSerialController::getNextFrame() {
    if (_data.isEmpty()) {
        return QByteArray();
    }
    if (_lastIndex >= -_data.count()) {
        _lastIndex = 0;
    }

    int startIndex = _lastIndex;
    _lastIndex += _count;

    return  _data.mid(startIndex, _count);
}

MainWindow::SendType FixedBytesSerialController::sendType() {
    return MainWindow::SendType ::FixedBytes;
}

FixedBytesSerialController::FixedBytesSerialController():SerialController() {

}
FixedBytesSerialController::FixedBytesSerialController(SerialController *serialController) : SerialController(
        serialController) {

}

int FixedBytesSerialController::getTotalCount() {
    return  _dataList.count();
}

int FixedBytesSerialController::setFixedCount(int count) {
    _count = count;
}

