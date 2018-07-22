//
// Created by chang on 2018-07-22.
//

#ifndef SERIALWIZARD_FIXEDCOUNTSERIALCONTROLLER_H
#define SERIALWIZARD_FIXEDCOUNTSERIALCONTROLLER_H


#include "SerialController.h"

class FixedBytesSerialController: public SerialController {
Q_OBJECT
public:
    explicit FixedBytesSerialController();
    explicit FixedBytesSerialController(SerialController *serialController);
    QByteArray getNextFrame() override;
    MainWindow::SendType sendType() override;

    int getTotalCount() override;

    int setFixedCount(int count);

private:
    int _lastIndex = 0;
    int _count = 128;
};


#endif //SERIALWIZARD_FIXEDCOUNTSERIALCONTROLLER_H
