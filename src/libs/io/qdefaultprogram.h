#ifndef QDefaultProgram_H
#define QDefaultProgram_H

#include "io_global.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include <QtCore/QSharedDataPointer>
#include <QtCore/QDebug>
#include <QtGui/QIcon>

class QDefaultProgramData;
class IO_EXPORT QDefaultProgram
{
public:
    QDefaultProgram();
    QDefaultProgram(const QDefaultProgram &);
    QDefaultProgram &operator=(const QDefaultProgram &);
    ~QDefaultProgram();

    bool isValid() const;

    QString comment() const;
    QString copyright() const;
    QString genericName() const;
    QIcon icon() const;
    QString identifier() const;
    QString name() const;
    QString path() const;
    QString version() const;

    static QDefaultProgram progamInfo(const QString &application);

    static QString defaultProgram(const QString &mimeType);
    static QString defaultProgram(const QUrl &url);
    static bool setDefaultProgram(const QString &mimeType, const QString &program);
//    static QStringList defaultPrograms(const QString &mimeType); // deprecated
    static QStringList defaultPrograms(const QUrl &url);

    static bool openUrlWith(const QUrl &url, const QString &application);
    static bool openUrlsWith(const QList<QUrl> &urls, const QString &application);

protected:
    explicit QDefaultProgram(const QDefaultProgramData &data);

private:
    QExplicitlySharedDataPointer<QDefaultProgramData> d;

    friend class QDefaultProgram;
};

#ifndef QT_NO_DEBUG_STREAM
QDebug IO_EXPORT operator<<(QDebug s, const QDefaultProgram &info);
#endif // QT_NO_DEBUG_STREAM

#endif // QDefaultProgram_H
