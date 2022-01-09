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

QString fromCodec(const QByteArray &data, QTextCodec *codec) {
    return codec->toUnicode(data);
}

QByteArray toCodecByteArray(const QString &text, QTextCodec *codec) {
    return codec->fromUnicode(text);
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

QByteArray dataToHex(const QByteArray &data, const QString &separator, const QString &prefix) {
    if (separator.isEmpty() && prefix.isEmpty()) {
        return data.toHex().toUpper();
    }

    QStringList list;
    auto len = data.count();
    for (int i = 0; i < len; i++) {
        list.append(prefix);
        auto hex = QString::number(data.at(i) & 0xFF, 16).toUpper();
        while (hex.size() < 2) {
            hex.prepend('0');
        }
        list.append(hex);
        if (i < len - 1) {
            list.append(separator);
        }
    }
    return list.join("").toUtf8();
}

QByteArray dataFromHex(const QString &data) {
    QRegExp rx("([0-9a-fA-F]{2})");
    QStringList list;
    auto pos = 0;
    while ((pos = rx.indexIn(data, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
    }

    if (list.isEmpty()) {
        return {};
    }
    auto line = list.join("").toLatin1();
    return QByteArray::fromHex(line);
}

QString getIpAddress(const QNetworkInterface &interface) {
    for (const auto &address: interface.addressEntries()) {
        if (address.ip().protocol() == QAbstractSocket::IPv4Protocol && !address.ip().toString().startsWith("169.")) {
            return address.ip().toString();
        }
    }
    return "";
}

QList<QNetworkInterface> getNetworkInterfaces() {
    QList<QNetworkInterface> list;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto &interface: interfaces) {
        if (interface.isValid() &&
            (interface.type() == QNetworkInterface::Wifi || interface.type() == QNetworkInterface::Ethernet)) {
            qDebug() << "interface " << interface.type() << interface.name() << interface.hardwareAddress();
            auto entries = interface.addressEntries();
            for (const auto &entry: entries) {
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

