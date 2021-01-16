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
    QByteArray readNextFrame() override;

    MainWindow::SendType sendType() override;
};


#endif //SERIALWIZARD_LINESERIALCONTROLLER_H
