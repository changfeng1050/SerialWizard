//
// Created by chang on 2018-03-07.
//

#include "TcpReadWriter.h"
#include <QTcpSocket>
#include <QTcpServer>

TcpReadWriter::TcpReadWriter(QObject *parent) : AbstractReadWriter(parent) {

}

bool TcpReadWriter::open() {
    close();
    _tcpServer = new QTcpServer(this);
    connect(_tcpServer, &QTcpServer::newConnection, this, [=] {
        if (_tcpSocket != nullptr) {
            _tcpSocket->close();
            delete _tcpSocket;
            _tcpSocket = nullptr;
        }
        _tcpSocket = _tcpServer->nextPendingConnection();
        connect(_tcpSocket, &QTcpSocket::readyRead, this, &TcpReadWriter::readyRead);
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

bool TcpReadWriter::isOpen() {
    return _tcpServer != nullptr && _tcpServer->isListening();
}

void TcpReadWriter::close() {
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

QByteArray TcpReadWriter::readAll() {
    if (_tcpSocket != nullptr && _tcpSocket->isOpen()) {
        return _tcpSocket->readAll();
    }
    qDebug() << "TcpReadWriter readAll() _tcpSocket == nullptr or not open";
    return QByteArray();
}

qint64 TcpReadWriter::write(const QByteArray &byteArray) const {
    if (_tcpSocket != nullptr && _tcpSocket->isOpen()) {
        return _tcpSocket->write(byteArray);
    }
    qDebug() << "TcpReadWriter write() _tcpSocket == nullptr or not open";
    return 0;
}

void TcpReadWriter::setAddress(const QString &address) {
    _address = address;
}

void TcpReadWriter::setPort(int port) {
    _port = port;
}

QString TcpReadWriter::settingsText() const {
    return QString("%1 %2").arg(_address).arg(_port);
}

bool TcpReadWriter::isConnected() {
    return isOpen() && _tcpSocket != nullptr && _tcpSocket->isOpen();
}
