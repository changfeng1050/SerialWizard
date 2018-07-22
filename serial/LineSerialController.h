//
// Created by chang on 2018-07-22.
//

#ifndef SERIALWIZARD_LINESERIALCONTROLLER_H
#define SERIALWIZARD_LINESERIALCONTROLLER_H


#include "SerialController.h"

class LineSerialController: public SerialController {
    Q_OBJECT
public:
    explicit LineSerialController();
    explicit LineSerialController(SerialController *serialController);
    QByteArray getNextFrame() override;

    MainWindow::SendType sendType() override;

    int getTotalCount() override;
};


#endif //SERIALWIZARD_LINESERIALCONTROLLER_H
