//
// Created by chang on 2018-02-06.
//

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "FileUtil.h"
#include <QDebug>
#include <QDir>
#include <QtCore/QCryptographicHash>
#include <QTextCodec>

qint64 getFileSize(const QString &filePath) {
    return QFile(filePath).size();
}

QByteArray getFileMd5(QString filePath) {
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error.";
        return 0;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (true) {
        if (bytesToWrite > 0) {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        } else {
            break;
        }

        if (bytesWritten == totalBytes) {
            break;
        }
    }

    localFile.close();
    QByteArray md5 = ch.result();
    return md5;
}

QString getFileName(const QString &filePath) {
    auto index = filePath.lastIndexOf('/');
    if (index == -1) {
        return filePath;
    }
    return filePath.right(filePath.count() - index - 1);
}

QString getFileNameWithoutSuffix(const QString &filePath) {
    auto index = filePath.lastIndexOf('/');
    auto endIndex = filePath.lastIndexOf('.');
    if (index == -1 && endIndex == -1) {
        return filePath;
    } else if (index == -1) {
        return filePath.left(endIndex);
    } else if (endIndex == -1) {
        return filePath.right(filePath.count() - index - 1);
    }

    return filePath.mid(index + 1, endIndex - index - 1);
}

QString getSuffix(const QString &filePath) {
    auto index = filePath.lastIndexOf('.');
    if (index == -1 || index == filePath.count() - 1) {
        return filePath;
    }
    return filePath.right(filePath.count() - index - 1);
}

QString getFileDir(const QString &filePath) {
    auto index = filePath.lastIndexOf('/');
    if (index == -1) {
        return "";
    }
    return filePath.left(index);
}

bool isVideoFile(const QString &filePath) {
    auto suffix = getSuffix(filePath).toLower();
    return suffix == "mp4"
           || suffix == "avi"
           || suffix == "ts"
           || suffix == "mpg"
           || suffix == "wmv"
           || suffix == "flv"
           || suffix == "mov"
           || suffix == "3gp"
           || suffix == "rmvb"
           || suffix == "rm"
           || suffix == "mkv"
           || suffix == "m4v";
}

bool isGifFile(const QString &filePath) {
    auto suffix = getSuffix(filePath).toLower();
    return suffix == "gif";
}

bool isImageFile(const QString &filePath) {
    auto suffix = getSuffix(filePath).toLower();
    return suffix == "png"
           || suffix == "jpg"
           || suffix == "jpeg"
           || suffix == "bmp";
}

bool isTextFile(const QString &filePath) {
    auto suffix = getSuffix(filePath).toLower();
    return suffix == "txt";
}

QByteArray readByteArray(const QString &path) {
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "getJsonFromFile failed";
        return "";
    }

    QByteArray byteArray;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        byteArray.append(line);
    }
    file.close();

    return byteArray;
}

QString readText(const QString &path) {
    return QString(readByteArray(path));
}

QJsonObject getJsonFromFile(const QString &path) {
    auto byteArray = readByteArray(path);
    return QJsonDocument::fromJson(byteArray).object();
}

bool writeFile(const QString &text, const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out.setCodec("utf-8");
    out << text;

    file.close();
    return true;
}

bool writeFile(const QByteArray &data, const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out.setCodec("utf-8");
    out << data;

    file.close();
    return true;
}

QString jsonToString(const QJsonObject &j) {
    return QJsonDocument(j).toJson();
}

bool saveJsonToFile(const QJsonObject &json, const QString &path) {
    QJsonDocument jd(json);
    return writeFile(jd.toJson(), path);
}

QString toPrettyJsonString(const QString &j) {
    QJsonParseError *error = new QJsonParseError();
    auto json = QJsonDocument::fromJson(j.toUtf8(), error);
    if (error->error != QJsonParseError::NoError) {
        return j;
    }

    return json.toJson();
}

QFileInfoList getFileList(const QString &path) {
    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    return fileList;
}

QString readFromFile(const QString &filePath) {
    auto ba = readByteArray(filePath);
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(ba.constData(), ba.size(), &state);
    qDebug() << "readFromFile" << state.invalidChars;
    if (state.invalidChars > 0) {
        QTextCodec *gbk = QTextCodec::codecForName("GB18030");
        text = gbk->toUnicode(ba).toUtf8();
    } else {
        text = ba;
    }
    return text;
}

