//
// Created by chang on 2018-03-07.
//

#include "TcpServerReadWriter.h"
#include <QTcpSocket>
#include <QTcpServer>

TcpServerReadWriter::TcpServerReadWriter(QObject *parent) : AbstractReadWriter(parent) {

}

bool TcpServerReadWriter::open() {
    close();
    _tcpServer = new QTcpServer(this);
    connect(_tcpServer, &QTcpServer::newConnection, this, [=] {
        if (_tcpSocket != nullptr) {
            _tcpSocket->close();
            delete _tcpSocket;
            _tcpSocket = nullptr;
        }
        _tcpSocket = _tcpServer->nextPendingConnection();
        connect(_tcpSocket, &QTcpSocket::readyRead, this, &TcpServerReadWriter::readyRead);
        connect(_tcpSocket, &QTcpSocket::disconnected, [this] {
            emit connectionClosed();
        });

        auto address = _tcpSocket->peerAddress().toString();
        if (address.contains(':')) {
            auto index = address.lastIndexOf(':');
            address = address.right(address.count() - index - 1);
        }
        auto port = _tcpSocket->peerPort();
        emit currentSocketChanged(address, port);
    });


    if (!_tcpServer->isListening()) {
        return _tcpServer->listen(QHostAddress::Any, static_cast<quint16>(_port));
    }
    return false;
}

bool TcpServerReadWriter::isOpen() {
    return _tcpServer != nullptr && _tcpServer->isListening();
}

void TcpServerReadWriter::close() {
    if (_tcpServer != nullptr) {
        if (_tcpServer->isListening()) {
            _tcpServer->close();
        }
        delete _tcpServer;
        _tcpServer = nullptr;
    }


    if (_tcpSocket != nullptr) {
        if (_tcpSocket->isOpen()) {
            _tcpSocket->close();
        }
//        delete _tcpSocket;
        _tcpSocket = nullptr;
    }
}

QByteArray TcpServerReadWriter::readAll() {
    if (_tcpSocket != nullptr && _tcpSocket->isOpen()) {
        return _tcpSocket->readAll();
    }
    qDebug() << "TcpServerReadWriter readAll() _tcpSocket == nullptr or not open";
    return {};
}

qint64 TcpServerReadWriter::write(const QByteArray &byteArray) const {
    if (_tcpSocket != nullptr && _tcpSocket->isOpen()) {
        return _tcpSocket->write(byteArray);
    }
    qDebug() << "TcpServerReadWriter write() _tcpSocket == nullptr or not open";
    return 0;
}

void TcpServerReadWriter::setAddress(const QString &address) {
    _address = address;
}

void TcpServerReadWriter::setPort(int port) {
    _port = port;
}

QString TcpServerReadWriter::settingsText() const {
    return QString("%1 %2").arg(_address).arg(_port);
}

bool TcpServerReadWriter::isConnected() {
    return isOpen() && _tcpSocket != nullptr && _tcpSocket->isOpen();
}
