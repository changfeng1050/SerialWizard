#include "serialport.h"

#include <QDebug>
#include <utility>

SerialPort::SerialPort(QObject *parent)
        : QObject(parent) {
    serial = nullptr;
}

SerialPort::SerialPort(SerialSettings settings, QObject *parent)
        : QObject(parent) {
    this->settings = new SerialSettings();
    *this->settings = std::move(settings);
}

SerialPort::~SerialPort() = default;

void SerialPort::setSerialSettings(SerialSettings settings) {
    this->settings = new SerialSettings();
    *this->settings = std::move(settings);
}

const SerialSettings & SerialPort::getSerialSettings() const {
    return *settings;
}
SerialOpenResult SerialPort::open() {

    if (serial != nullptr && serial->isOpen()) {
        serial->close();
    }

    serial = new QSerialPort(this);
    serial->setPortName(settings->name);
    serial->setBaudRate(settings->baudRate);
    serial->setDataBits(settings->dataBits);
    serial->setParity(settings->parity);
    serial->setStopBits(settings->stopBits);
    serial->setFlowControl(settings->flowControl);


    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << serial->portName() << serial->baudRate() << serial->dataBits() << serial->parity()
                 << serial->stopBits() << serial->flowControl();
        connect(serial, &QSerialPort::readyRead, [this]() {
            QByteArray data = this->serial->readAll();
            emit this->dataReceived(data);
            byteCount += data.length();

        });
        return SerialOpenResultSuccess;
    } else {
        return SerialOpenResultFailed;
    }
}

void SerialPort::close() {
    if (serial != nullptr) {
        serial->close();
        serial = nullptr;
    }
}

qint64 SerialPort::write(QByteArray &data) {
    if (serial == nullptr) {
        return 0;
    }
    auto ret = serial->write(data);
    if (ret > 0) {
        emit dataSent(data);
    }

    return ret;
}
//
//qint64 SerialPort::write(const char *data) {
//    if (serial == nullptr) {
//        return 0;
//    }
//    return serial->write(data);
//}
//
//qint64 SerialPort::write(const char *data, qint64 len) {
//    if (serial == nullptr) {
//        return 0;
//    }
//    return serial->write(data, len);
//}

bool SerialPort::isOpen() {
    return serial != nullptr && serial->isOpen();
}





