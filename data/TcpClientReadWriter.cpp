//
// Created by chang on 2019-02-09.
//
#include <QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QHostAddress>

#include "TcpClientReadWriter.h"

TcpClientReadWriter::TcpClientReadWriter(QObject *parent) : AbstractReadWriter(parent) {

}

bool TcpClientReadWriter::open() {
    close();
    _tcpSocket = new QTcpSocket();
    connect(_tcpSocket, &QTcpSocket::readyRead, this, &AbstractReadWriter::readyRead);
    connect(_tcpSocket, &QTcpSocket::connected, [] {
        qDebug() << "connected";
    });

    qDebug() << "TcpClientReadWriter type:" << _tcpSocket->socketType() << _address << _port;

    connect(_tcpSocket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
            [](QAbstractSocket::SocketError error) {
                qDebug() << "error" << error;
            });
    _tcpSocket->connectToHost(_address, _port);
    auto connected = _tcpSocket->waitForConnected();
    return _tcpSocket->isOpen() && connected;
}

bool TcpClientReadWriter::isOpen() {
    return _tcpSocket != nullptr && _tcpSocket->isOpen();
}

void TcpClientReadWriter::close() {
    if (_tcpSocket != nullptr) {
        qDebug() << "close";
        _tcpSocket->close();
        delete _tcpSocket;
        _tcpSocket = nullptr;
    }
}

QByteArray TcpClientReadWriter::readAll() {
    if (_tcpSocket != nullptr && _tcpSocket->isOpen()) {
        return _tcpSocket->readAll();
    }
    qDebug() << "TcpClientReadWriter readAll() _tcpSocket == nullptr or not open";
    return {};
}

qint64 TcpClientReadWriter::write(const QByteArray &byteArray) const {
    if (_tcpSocket != nullptr && _tcpSocket->isOpen()) {
        return _tcpSocket->write(byteArray);
    }
    qDebug() << "TcpClientReadWriter write() _tcpSocket == nullptr or not open";
    return -1;
}

void TcpClientReadWriter::setAddress(const QString &address) {
    _address = address;
}

void TcpClientReadWriter::setPort(int port) {
    _port = port;
}

QString TcpClientReadWriter::settingsText() const {
    return QString("%1 %2").arg(_address).arg(_port);
}

bool TcpClientReadWriter::isConnected() {
    return isOpen() && _tcpSocket != nullptr && _tcpSocket->isOpen();
}
