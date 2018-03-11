//
// Created by chang on 2018-03-07.
//

#include "SerialReadWriter.h"
#include <QDebug>

SerialReadWriter::SerialReadWriter(QObject *parent) : AbstractReadWriter(parent) {

}

bool SerialReadWriter::open() {
    close();

    serial = new QSerialPort(this);
    serial->setPortName(settings.name);
    serial->setBaudRate(settings.baudRate);
    serial->setDataBits(settings.dataBits);
    serial->setParity(settings.parity);
    serial->setStopBits(settings.stopBits);
    serial->setFlowControl(settings.flowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        connect(serial, &QSerialPort::readyRead, this, &SerialReadWriter::readyRead);
        return true;
    } else {
        return false;
    }
}

bool SerialReadWriter::isOpen() {
    return serial != nullptr && serial->isOpen();
}

QByteArray SerialReadWriter::readAll() {
    if (serial != nullptr && serial->isOpen()) {
        return serial->readAll();
    }
    qDebug() << "SerialReadWriter readAll() _serial == nullptr or not open";
    return QByteArray();
}

qint64 SerialReadWriter::write(const QByteArray &byteArray) const {
    if (serial != nullptr && serial->isOpen()) {
        return serial->write(byteArray);
    }
    qDebug() << "SerialReadWriter readAll() _serial == nullptr or not open";
    return 0;
}

void SerialReadWriter::setSerialSettings(SerialSettings serialSettings) {
    this->settings = std::move(serialSettings);
}

void SerialReadWriter::close() {
    if (serial != nullptr) {
        serial->close();
        delete serial;
        serial = nullptr;
    }
}

QString SerialReadWriter::settingsText() const {
    return QString("%1 %2 %3 %4 %5").arg(settings.name).arg(settings.baudRate).arg(settings.dataBits).arg(
            settings.stopBits).arg(settings.parity);
}

bool SerialReadWriter::isConnected() {
    return serial != nullptr && serial->isOpen();
}
