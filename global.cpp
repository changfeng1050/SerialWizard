//
// Created by chang on 2017-08-02.
//

#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QTime>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkInterface>
#include "global.h"

QTextCodec *gbk = QTextCodec::codecForName("GB18030");
QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

QString utf82Gbk(const QString &inStr) {
    //    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    //    gbk->fromUnicode(utf8->toUnicode(inStr.toLatin1()));

    return QString(gbk->fromUnicode(inStr));

    //    QString utf2gbk = gbk->toUnicode(inStr.toLocal8Bit());
    //    return utf2gbk;
}
//
//QString gbk2Utf8(const QString &inStr) {
//    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
//    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
//
//    gbk->fromUnicode(utf8->toUnicode(inStr.toLatin1()));
//
//    return QString(gbk->fromUnicode(inStr));
//}

QString fromUtf8(const QByteArray &data) {
    qDebug() << "fromUtf8" << data.toHex();
    return utf8->toUnicode(data);
}

QString fromGbk(const QByteArray &data) {
    qDebug() << "fromGbk" << data.toHex();
    return gbk->toUnicode(data);
}

QByteArray toGbkByteArray(const QString &text) {
    qDebug() << "toGbkByteArray" << text;
    return text.toLocal8Bit();
}

QByteArray toUtf8ByteArray(const QString &text) {
    qDebug() << "toUtf8ByteArray" << text;
    return text.toUtf8();
}

bool okToContinue(const QString &title, const QString &text, QWidget *parent) {
    return QMessageBox::Yes == QMessageBox::warning(parent, title, text, QMessageBox::Yes | QMessageBox::No);
}

bool showQuestion(const QString &title, const QString &text, QWidget *parent) {
    return QMessageBox::question(parent, title, text, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

void showError(const QString &title, const QString &text, QWidget *parent) {
    return (void) QMessageBox::critical(parent, title, text, QMessageBox::Ok);
}

bool showWarning(const QString &title, const QString &text, QWidget *parent) {
    return QMessageBox::warning(parent, title, text, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

void showMessage(const QString &title, const QString &text, QWidget *parent) {
    return (void) QMessageBox::information(parent, title, text);
}

QString getTimestamp() {
    auto time = QTime(QTime::currentTime());
    return time.toString("hh:mm:ss.zzz");
}

QString getIp() {
    auto interfaces = QNetworkInterface::allInterfaces();

    for (const auto &interface:interfaces) {
        qDebug() << "interface name:" << interface.name();
        qDebug() << "interface address:" << interface.hardwareAddress();
        qDebug() << "interface type:" << interface.type();
        auto entries = interface.addressEntries();

        for (const auto &entry:entries) {
            qDebug() << "entry ip address:" << entry.ip().toString();
        }
    }

    auto localHostName = QHostInfo::localHostName();
    qDebug() << "local host name:" << localHostName;
    auto ipAddress = QHostInfo::fromName(localHostName).addresses();
    qDebug() << "ip address count:" << ipAddress.length();
    qDebug() << "ip address:" << ipAddress;

    for (const auto &address:ipAddress) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            return address.toString();
        }
    }
    for (const auto &address:ipAddress) {
        if (address.protocol() == QAbstractSocket::IPv6Protocol) {
            return address.toString();
        }
    }

    return "";
}

QString getAddressString(const QHostAddress &address) {
    return address.toString();
}

QByteArray dataToHex(const QByteArray &data) {
    QByteArray result = data.toHex().toUpper();

    for (int i = 2; i < result.size(); i += 3)
        result.insert(i, ' ');

    return result;
}

QByteArray dataFromHex(const QString &hex) {
    QRegExp rx("(([0-9a-fA-F]{2}\\s?)+)");
    rx.indexIn(hex.simplified());
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(hex, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
    }

    QString m;
    // 查找最匹配的
    for (const auto &text:list) {
        qDebug() << "dataFromHex: text:" << text << endl;
        if (text.count() > m.count()) {
            m = text;
        }
    }
    auto result1 = m.trimmed();
    qDebug() << "dataFromHex" << hex << ">>" << result1 << endl;
    QByteArray line = result1.toLatin1();
    line.replace(' ', QByteArray());
    auto result = QByteArray::fromHex(line);
    return result;
}

QString getIpAddress(const QNetworkInterface &interface) {
    for (const auto &address:interface.addressEntries()) {
        if (address.ip().protocol() == QAbstractSocket::IPv4Protocol && !address.ip().toString().startsWith("169.")) {
            return address.ip().toString();
        }
    }
    return "";
}

QList<QNetworkInterface> getNetworkInterfaces() {
    QList<QNetworkInterface> list;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto &interface:interfaces) {
        if (interface.isValid() &&
            (interface.type() == QNetworkInterface::Wifi || interface.type() == QNetworkInterface::Ethernet)) {
            qDebug() << "interface " << interface.type() << interface.name() << interface.hardwareAddress();
            auto entries = interface.addressEntries();
            for (const auto &entry:entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    qDebug() << "interface ip:" << entry.ip().toString();
                }
            }
            list.append(interface);
        }
    }
    return list;
}

QStringList getLines(const QString &text) {
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    return lines;
}

