//
// Created by chang on 2019-07-11.
//

#include "SerialBridgeReadWriter.h"

SerialBridgeReadWriter::SerialBridgeReadWriter(QObject *parent) : AbstractReadWriter(parent) {
    _serialReadWriter1 = new SerialReadWriter(parent);
    _serialReadWriter2 = new SerialReadWriter(parent);

    connect(_serialReadWriter1, &SerialReadWriter::readyRead, [this] {
        auto dataRead = _serialReadWriter1->readAll();
        _serialReadWriter2->write(dataRead);
        emit serial1DataRead(dataRead);
    });

    connect(_serialReadWriter2, &SerialReadWriter::readyRead, [this] {
        auto dataRead = _serialReadWriter2->readAll();
        _serialReadWriter1->write(dataRead);
        emit serial2DataRead(dataRead);
    });
}

void SerialBridgeReadWriter::setSettings(SerialSettings serialSettings1, SerialSettings serialSettings2) {
    _serialReadWriter1->setSerialSettings(std::move(serialSettings1));
    _serialReadWriter2->setSerialSettings(std::move(serialSettings2));
}

QString SerialBridgeReadWriter::settingsText() const {
    return _serialReadWriter1->settingsText() + " " + _serialReadWriter2->settingsText();
}

bool SerialBridgeReadWriter::open() {
    if (!_serialReadWriter1->open()) {
        return false;
    }
    if (!_serialReadWriter2->open()) {
        return false;
    }
    return true;
}

bool SerialBridgeReadWriter::isOpen() {
    return _serialReadWriter1->isOpen() && _serialReadWriter2->isOpen();
}

void SerialBridgeReadWriter::close() {
    _serialReadWriter1->close();
    _serialReadWriter2->close();
}

QByteArray SerialBridgeReadWriter::readAll() {
    return  _serialReadWriter1->readAll();
}

qint64 SerialBridgeReadWriter::write(const QByteArray &byteArray) const {
    return _serialReadWriter2->write(byteArray);
}

bool SerialBridgeReadWriter::isConnected() {
    return _serialReadWriter1->isConnected() && _serialReadWriter2->isConnected();
}
