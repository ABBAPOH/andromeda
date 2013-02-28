#ifndef QDEFAULT_PROGRAM_H
#define QDEFAULT_PROGRAM_H

#include "io_global.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include <QtCore/QSharedDataPointer>
#include <QtCore/QDebug>
#include <QtGui/QIcon>

#define NO_DEFAULT_PROGRAM_FOR_MIMETYPE

class QDefaultProgram;
typedef QList<QDefaultProgram> QDefaultProgramList;

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

    bool operator==(const QDefaultProgram &other) const;
    inline bool operator!=(const QDefaultProgram &other) const { return !operator==(other); }

    static QDefaultProgram defaultProgram(const QUrl &url);
#ifndef NO_DEFAULT_PROGRAM_FOR_MIMETYPE
    static QDefaultProgram defaultProgram(const QString &mimeType);
    static bool setDefaultProgram(const QString &mimeType, const QString &program);
#endif
    static QDefaultProgramList programsForUrl(const QUrl &url);
    static QDefaultProgramList programsForUrls(const QList<QUrl> &urls);

    bool open(const QUrl &url) const;
    bool open(const QList<QUrl> &urls) const;

public:
    explicit QDefaultProgram(const QDefaultProgramData &data);

private:
    QExplicitlySharedDataPointer<QDefaultProgramData> d;
};

#ifndef QT_NO_DEBUG_STREAM
QDebug IO_EXPORT operator<<(QDebug s, const QDefaultProgram &info);
#endif // QT_NO_DEBUG_STREAM

#endif // QDEFAULT_PROGRAM_H
