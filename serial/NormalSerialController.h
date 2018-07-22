//
// Created by chang on 2018-07-22.
//

#ifndef SERIALWIZARD_NORMALSERIALCONTROLLER_H
#define SERIALWIZARD_NORMALSERIALCONTROLLER_H


#include "SerialController.h"

class NormalSerialController: public SerialController {
   Q_OBJECT
public:
    explicit NormalSerialController();
    explicit NormalSerialController(SerialController *serialController);

    QByteArray getNextFrame() override;

    MainWindow::SendType sendType() override;

    int getTotalCount() override;
};


#endif //SERIALWIZARD_NORMALSERIALCONTROLLER_H
