#ifndef QDEFAULTPROGRAMS_H
#define QDEFAULTPROGRAMS_H

#include "io_global.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include "qprograminfo.h"

class IO_EXPORT QDefaultPrograms
{
public:
    QDefaultPrograms();

    static QProgramInfo progamInfo(const QString &application);

    static QString defaultProgram(const QString &mimeType);
    static QString defaultProgram(const QUrl &url);
    static bool setDefaultProgram(const QString &mimeType, const QString &program);
//    static QStringList defaultPrograms(const QString &mimeType); // deprecated
    static QStringList defaultPrograms(const QUrl &url);

    static bool openUrlWith(const QUrl &url, const QString &application);
    static bool openUrlsWith(const QList<QUrl> &urls, const QString &application);
};

#endif // QDEFAULTPROGRAMS_H
