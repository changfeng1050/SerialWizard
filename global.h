//
// Created by chang on 2017-08-02.
//

#ifndef SERIALWIZARD_GLOBAL_H
#define SERIALWIZARD_GLOBAL_H

extern QString utf82Gbk(const QString &inStr);

extern QString getTimestamp();


extern bool okToContinue(const QString &title, const QString &text, QWidget *parent = nullptr);

extern bool showQuestion(const QString &title, const QString &text, QWidget *parent = nullptr);

extern void showError(const QString &title, const QString &text, QWidget *parent = nullptr);

extern bool showWarning(const QString &title, const QString &text, QWidget *parent = nullptr);

extern void showMessage(const QString &title, const QString &text, QWidget *parent = nullptr);


#endif //SERIALWIZARD_GLOBAL_H
