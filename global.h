//
// Created by chang on 2017-08-02.
//

#ifndef SERIALWIZARD_GLOBAL_H
#define SERIALWIZARD_GLOBAL_H

#include <QString>
#include <QWidget>

extern QString utf82Gbk(const QString &inStr);

extern QString fromUtf8(const QByteArray &data);

extern QString fromGbk(const QByteArray &data);

extern QByteArray toGbkByteArray(const QString &text);

extern QByteArray toUtf8ByteArray(const QString &text);

extern QString getTimestamp();


extern bool okToContinue(const QString &title, const QString &text, QWidget *parent = nullptr);

extern bool showQuestion(const QString &title, const QString &text, QWidget *parent = nullptr);

extern void showError(const QString &title, const QString &text, QWidget *parent = nullptr);

extern bool showWarning(const QString &title, const QString &text, QWidget *parent = nullptr);

extern void showMessage(const QString &title, const QString &text, QWidget *parent = nullptr);

extern QString getFileSuffix(const QString &filePath);

extern QString getFileDir(const QString &filePath);

extern QString getIp();

extern QByteArray dataToHex(const QByteArray &data);

extern QByteArray dataFromHex(const QString &data);


#endif //SERIALWIZARD_GLOBAL_H
