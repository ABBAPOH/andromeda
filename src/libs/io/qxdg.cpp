#include "qxdg.h"

#include <QtCore/QDir>
#include <QtCore/QDebug>

static inline QString homePath()
{
    return QString(qgetenv("HOME"));
}

static inline void fixBashShortcuts(QString &s)
{
    if (s.startsWith('~'))
        s = QString(qgetenv("HOME")) + (s).mid(1);
}

static QString xdgSingleDir(const QString &envVar, const QString &def, bool createDir)
{
    QString s(qgetenv(envVar.toAscii()));

    if (!s.isEmpty())
        fixBashShortcuts(s);
    else
        s = QString("%1/%2").arg(qgetenv("HOME"), def);

    QDir d(s);
    if (createDir && !d.exists()) {
        if (!d.mkpath("."))
            qWarning() << QString("Can't create %1 directory.").arg(d.absolutePath());
    }

    return d.absolutePath();
}

QString dataHome(bool createDir)
{
    return xdgSingleDir("XDG_DATA_HOME", ".local/share", createDir);
}
