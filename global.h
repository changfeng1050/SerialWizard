//
// Created by chang on 2017-08-02.
//

#ifndef SERIALWIZARD_GLOBAL_H
#define SERIALWIZARD_GLOBAL_H

#include <QString>
#include <QWidget>
#include <QtNetwork/QNetworkInterface>

extern QString fromCodec(const QByteArray &data, QTextCodec *codec);

extern QByteArray toCodecByteArray(const QString &data, QTextCodec *codec);

extern QString getTimestamp();

extern bool okToContinue(const QString &title, const QString &text, QWidget *parent = nullptr);

extern bool showQuestion(const QString &title, const QString &text, QWidget *parent = nullptr);

extern void showError(const QString &title, const QString &text, QWidget *parent = nullptr);

extern bool showWarning(const QString &title, const QString &text, QWidget *parent = nullptr);

extern void showMessage(const QString &title, const QString &text, QWidget *parent = nullptr);

extern QString getFileSuffix(const QString &filePath);

extern QString getFileDir(const QString &filePath);

[[maybe_unused]] extern QString getIp();

extern QString getIpAddress(const QNetworkInterface &interface);

extern QList<QNetworkInterface> getNetworkInterfaces();

extern QByteArray dataToHex(const QByteArray &data, const QString &separator = " ", const QString &prefix = "");

extern QByteArray dataFromHex(const QString &data);

extern QStringList getLines(const QString &text);

#endif //SERIALWIZARD_GLOBAL_H
