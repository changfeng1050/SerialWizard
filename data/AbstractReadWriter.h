//
// Created by chang on 2018-03-07.
//

#ifndef SERIALWIZARD_ABSTRACTREADWRITER_H
#define SERIALWIZARD_ABSTRACTREADWRITER_H


#include <QtCore/QObject>

class AbstractReadWriter : public QObject {
Q_OBJECT
public:
    explicit AbstractReadWriter(QObject *parent = nullptr);

    virtual bool open() = 0;

    virtual bool isOpen() = 0;

    virtual bool isConnected() = 0;

    virtual void close() = 0;

    virtual QByteArray readAll() = 0;

    virtual qint64 write(const QByteArray &byteArray) const = 0;

    virtual QString settingsText() const = 0;

signals:
    void readyRead();
};


#endif //SERIALWIZARD_ABSTRACTREADWRITER_H
