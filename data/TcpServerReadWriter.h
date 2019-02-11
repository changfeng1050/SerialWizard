//
// Created by chang on 2018-03-07.
//

#ifndef SERIALWIZARD_TCPREADWRITER_H
#define SERIALWIZARD_TCPREADWRITER_H


#include "AbstractReadWriter.h"

class QTcpServer;

class QTcpSocket;

class TcpServerReadWriter : public AbstractReadWriter {
Q_OBJECT
public:
    explicit TcpServerReadWriter(QObject *parent = nullptr);

    bool open() override;

    bool isOpen() override;

    bool isConnected() override;

    QString settingsText() const override;

    void close() override;

    void setAddress(const QString &address);

    void setPort(int port);

    QByteArray readAll() override;

    qint64 write(const QByteArray &byteArray) const override;

signals:
    void currentSocketChanged(const QString &address, qint16 port);
    void connectionClosed();

private:
    QTcpServer *_tcpServer{nullptr};
    QTcpSocket *_tcpSocket{nullptr};
    QString _address{"192.168.1.100"};
    int _port{9000};
};


#endif //SERIALWIZARD_TCPREADWRITER_H
