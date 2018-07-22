//
// Created by chang on 2018-07-22.
//

#include "NormalSerialController.h"

QByteArray NormalSerialController::getNextFrame() {
    return _data;
}

MainWindow::SendType NormalSerialController::sendType() {
    return MainWindow::SendType ::Normal;
}

NormalSerialController::NormalSerialController():SerialController() {

}

NormalSerialController::NormalSerialController(SerialController *serialController) : SerialController(
        serialController) {

}

int NormalSerialController::getTotalCount() {
    return _dataList.count();
}
