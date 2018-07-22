//
// Created by chang on 2018-07-22.
//

#include <FrameInfoDialog.h>
#include "FrameSerialController.h"

QByteArray FrameSerialController::getNextFrame() {
    if (_data.isEmpty()){
        return QByteArray();
    }

    if (_frameInfo == nullptr) {
        return QByteArray();
    }

    if (_lastIndex >= _data.count()) {
        _lastIndex =0;
    }

    int startIndex = _lastIndex;

    int headIndex = _data.indexOf(_frameInfo->head, startIndex);
    int endIndex = _data.indexOf(_frameInfo->end, startIndex + 1);

    _lastIndex = endIndex;

    auto d = _data.mid(headIndex, endIndex - startIndex + 1);
    if (d.isEmpty()) {
        _lastIndex =0;
    }

    return d;
}

MainWindow::SendType FrameSerialController::sendType() {
    return MainWindow::SendType ::Frame;
}

FrameSerialController::FrameSerialController(SerialController *serialController) : SerialController(serialController) {

}

FrameSerialController::FrameSerialController() {

}

int FrameSerialController::getTotalCount() {
    return _dataList.count();
}

void FrameSerialController::setFrameInfo(const FrameInfo &frameInfo) {
    if (_frameInfo == nullptr) {
       _frameInfo = new FrameInfo();
    }
    *_frameInfo = frameInfo;
}

