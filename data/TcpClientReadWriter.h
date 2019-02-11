//
// Created by chang on 2019-02-09.
//

#ifndef SERIALWIZARD_TCPCLIENTREADWRITER_H
#define SERIALWIZARD_TCPCLIENTREADWRITER_H

#include <QTcpSocket>

#include "AbstractReadWriter.h"

class TcpClientReadWriter : public AbstractReadWriter {
Q_OBJECT
public:
    explicit TcpClientReadWriter(QObject *parent = nullptr);

    bool open() override;

    bool isOpen() override;

    bool isConnected() override;

    QString settingsText() const override;

    void setAddress(const QString &address);

    void setPort(int port);

    QByteArray readAll() override;

    qint64 write(const QByteArray &byteArray) const override;

    void close() override;
private:
    QTcpSocket *_tcpSocket{nullptr};
    QString _address{"192.168.1.100"};
    int _port{9000};

};


#endif //SERIALWIZARD_TCPCLIENTREADWRITER_H
