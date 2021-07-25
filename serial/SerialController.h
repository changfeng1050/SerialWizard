//
// Created by chang on 2018-07-22.
//

#ifndef SERIALWIZARD_SERIALCONTROLLER_H
#define SERIALWIZARD_SERIALCONTROLLER_H

#include <QtCore/QObject>
#include <mainwindow.h>

class SerialController : public QObject {
Q_OBJECT
public:

    explicit SerialController();

    explicit SerialController(SerialController *serialController);

    ~SerialController() override;

    void setData(const QList<QByteArray> &dataList);

    virtual QByteArray readNextFrame() = 0;

    int getTotalCount();

    int getCurrentCount();

    void setCurrentCount(int count);

    void increaseCurrentCount();

    void decreaseCurrentCount();

    bool readEnd();

    virtual MainWindow::SendType sendType() = 0;

protected:

    QList<QByteArray> _dataList;

    int _currentCount{0};
};

#endif //SERIALWIZARD_SERIALCONTROLLER_H
