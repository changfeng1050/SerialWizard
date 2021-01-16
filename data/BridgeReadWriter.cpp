//
// Created by chang on 2018-03-11.
//

#include "BridgeReadWriter.h"

#include "TcpServerReadWriter.h"

BridgeReadWriter::BridgeReadWriter(QObject *parent) : AbstractReadWriter(parent) {
    _serialReaderWriter = new SerialReadWriter(parent);
    _tcpReaderWriter = new TcpServerReadWriter(parent);


    connect(_serialReaderWriter, &SerialReadWriter::readyRead, [this] {
        auto dataRead = _serialReaderWriter->readAll();
        _tcpReaderWriter->write(dataRead);
        emit serialDataRead(dataRead);
    });

    connect(_tcpReaderWriter, &TcpServerReadWriter::readyRead, [this] {
        auto dataRead = _tcpReaderWriter->readAll();
        _serialReaderWriter->write(dataRead);
        emit tcpDataRead(dataRead);
    });

    connect(_tcpReaderWriter, &TcpServerReadWriter::currentSocketChanged, this, &BridgeReadWriter::currentSocketChanged);
    connect(_tcpReaderWriter, &TcpServerReadWriter::connectionClosed, this, &BridgeReadWriter::connectionClosed);
}

void BridgeReadWriter::setSettings(SerialSettings serialSettings, const QString &tcpAddress, qint16 port) {
    _serialReaderWriter->setSerialSettings(std::move(serialSettings));
    _tcpReaderWriter->setAddress(tcpAddress);
    _tcpReaderWriter->setPort(port);
}

QString BridgeReadWriter::settingsText() const {
    return _serialReaderWriter->settingsText() + " " + _tcpReaderWriter->settingsText();
}

bool BridgeReadWriter::open() {
    if (!_serialReaderWriter->open()) {
        return false;
    }

    if (!_tcpReaderWriter->open()) {
        return false;
    }
    return true;
}

bool BridgeReadWriter::isOpen() {
    return _serialReaderWriter->isOpen() && _tcpReaderWriter->isOpen();
}

void BridgeReadWriter::close() {
    _serialReaderWriter->close();
    _tcpReaderWriter->close();
}

QByteArray BridgeReadWriter::readAll() {
    return _tcpReaderWriter->readAll();
}

qint64 BridgeReadWriter::write(const QByteArray &byteArray) const {
    return _tcpReaderWriter->write(byteArray);
}

bool BridgeReadWriter::isConnected() {
    return _serialReaderWriter->isConnected() && _tcpReaderWriter->isConnected();
}
