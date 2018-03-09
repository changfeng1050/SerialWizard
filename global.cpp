//
// Created by chang on 2017-08-02.
//

#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QTime>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QHostInfo>
#include "global.h"

QString utf82Gbk(const QString &inStr) {
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

    gbk->fromUnicode(utf8->toUnicode(inStr.toLatin1()));

    return QString(gbk->fromUnicode(inStr));

//    QString utf2gbk = gbk->toUnicode(inStr.toLocal8Bit());
//    return utf2gbk;
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
};

QString getTimestamp() {
    auto time = QTime(QTime::currentTime());
    return time.toString("hh:mm:ss.zzz");
}

QString getFileSuffix(const QString &filePath) {
    auto index = filePath.lastIndexOf('.');
    return filePath.right(filePath.count() - index - 1);
}

QString getFileDir(const QString &filePath) {
    auto index = filePath.lastIndexOf('.');
    return filePath.left(index + 1);
}

QString getIp() {
    auto localHostName = QHostInfo::localHostName();
    qDebug() << "local host name:" << localHostName;
    auto ipAddress = QHostInfo::fromName(localHostName).addresses();
    qDebug() << "ip address:" << ipAddress;

    for (auto address:ipAddress) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            return address.toString();
        }
    }
    for (auto address:ipAddress) {
        if (address.protocol() == QAbstractSocket::IPv6Protocol) {
            return address.toString();
        }
    }

    return "";
}

QString getAddressString(const QHostAddress &address) {
    return address.toString();
}
