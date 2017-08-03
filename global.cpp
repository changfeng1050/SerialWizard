//
// Created by chang on 2017-08-02.
//

#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QTime>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include "global.h"
QString utf82Gbk(const QString &inStr)
{
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
