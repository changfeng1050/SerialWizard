//
// Created by chang on 2018-07-22.
//

#ifndef SERIALWIZARD_FRAMESERIALCONTROLLER_H
#define SERIALWIZARD_FRAMESERIALCONTROLLER_H

#include "FrameInfoDialog.h"
#include "SerialController.h"


class FrameSerialController : public SerialController{
Q_OBJECT
public:
    explicit FrameSerialController();
    explicit FrameSerialController(SerialController *serialController);
    QByteArray getNextFrame() override;

    MainWindow::SendType sendType() override;

    int getTotalCount() override;

    void setFrameInfo(const FrameInfo &frameInfo);

private:
    FrameInfo *_frameInfo;
    int _lastIndex = 0;
};


#endif //SERIALWIZARD_FRAMESERIALCONTROLLER_H
