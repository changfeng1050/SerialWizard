#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

struct SerialSettings{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
    bool localEchoEnabled;
};


enum SerialOpenResult
{
    SerialOpenResultParamNotSet=-1,
    SerialOpenResultSuccess,
    SerialOpenResultFailed,
};



class SerialPort : public QObject
{
    Q_OBJECT
public:
    SerialPort(QObject *parent = nullptr);
    SerialPort(SerialSettings settings, QObject *parent = nullptr);
    ~SerialPort();

    void setSerialSettings(SerialSettings settings);
    const SerialSettings & getSerialSettings() const;
    SerialOpenResult open();
    qint64 write(QByteArray &data);
//    qint64 write(const char* data);
//    qint64 write(const char *data, qint64 len);
    void close();

    bool isOpen();


signals:
    void dataReceived(const QByteArray &data);
    void receivedByteCountChanged(qint64 count);
    void dataSent(const QByteArray &data);

private:
    qint64 byteCount;
    SerialSettings *settings;
    QSerialPort *serial;



};

#endif // SERIALPORT_H
