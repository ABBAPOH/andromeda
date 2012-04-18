#ifndef QPROGRAMINFO_H
#define QPROGRAMINFO_H

#include "io_global.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QDebug>
#include <QtGui/QIcon>

class QProgramInfoData;
class IO_EXPORT QProgramInfo
{
public:
    QProgramInfo();
    QProgramInfo(const QProgramInfo &);
    QProgramInfo &operator=(const QProgramInfo &);
    ~QProgramInfo();

    bool isValid() const;

    QString comment() const;
    QString copyright() const;
    QString genericName() const;
    QIcon icon() const;
    QString identifier() const;
    QString name() const;
    QString path() const;
    QString version() const;

protected:
    explicit QProgramInfo(const QProgramInfoData &data);

private:
    QExplicitlySharedDataPointer<QProgramInfoData> d;

    friend class QDefaultPrograms;
};

QDebug IO_EXPORT operator<<(QDebug s, const QProgramInfo &info);

#endif // QPROGRAMINFO_H
