//
// Created by chang on 2018-02-06.
//

#ifndef WIZARDEDITOR_FILEUTIL_H
#define WIZARDEDITOR_FILEUTIL_H

#include <QtCore/QString>
#include <QJsonObject>
#include <QtCore/QFileInfoList>

extern qint64 getFileSize(const QString &filePath);

extern QByteArray getFileMd5(QString filePath);

extern QString getFileName(const QString &filePath);

extern QString getFileNameWithoutSuffix(const QString &fileName);

extern QString getSuffix(const QString &filePath);

extern QString getFileDir(const QString &filePath);

extern bool isVideoFile(const QString &filePath);

extern bool isGifFile(const QString &filePath);

extern bool isImageFile(const QString &filePath);

extern bool isTextFile(const QString &filePath);

extern bool saveJsonToFile(const QJsonObject &json, const QString &path);

extern bool writeFile(const QString &text, const QString &path);

extern bool writeFile(const QByteArray &data, const QString &path);

extern QByteArray readByteArray(const QString &path);

extern QString readText(const QString &path);

extern QJsonObject getJsonFromFile(const QString &path);

extern QString jsonToString(const QJsonObject &j);

extern QFileInfoList getFileList(const QString &path);

extern QString toPrettyJsonString(const QString &j);

extern QString readFromFile(const QString &filePath);

#endif //WIZARDEDITOR_FILEUTIL_H
