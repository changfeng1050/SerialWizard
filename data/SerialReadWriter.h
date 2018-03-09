//
// Created by chang on 2018-03-07.
//

#ifndef SERIALWIZARD_SERIALREADWRITER_H
#define SERIALWIZARD_SERIALREADWRITER_H

#include <QtSerialPort/QSerialPort>
#include "AbstractReadWriter.h"

struct SerialSettings {
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
    bool localEchoEnabled;
};


class SerialReadWriter : public AbstractReadWriter {
Q_OBJECT
public:
    explicit SerialReadWriter(QObject *parent = nullptr);

    void setSerialSettings(SerialSettings serialSettings);

    QString settingsText() const override;

    bool open() override;

    bool isOpen() override;

    bool isConnected() override;

    void close() override;

    QByteArray readAll() override;

    qint64 write(const QByteArray &byteArray) const override;

private:
    SerialSettings settings;
    QSerialPort *serial{nullptr};
};


#endif //SERIALWIZARD_SERIALREADWRITER_H
