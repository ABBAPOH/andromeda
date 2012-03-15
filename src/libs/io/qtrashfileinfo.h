#ifndef QTRASHFILEINFO_H
#define QTRASHFILEINFO_H

#include "io_global.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QDateTime>

class QTrashFileInfoData;

class IO_EXPORT QTrashFileInfo
{
public:
    QTrashFileInfo();
    QTrashFileInfo(const QTrashFileInfo &);
    QTrashFileInfo &operator=(const QTrashFileInfo &);
    ~QTrashFileInfo();

    QString name() const;
    QString path() const;

    QString originalName() const;
    QString originalPath() const;

    QDateTime deletionDateTime() const;

    qint64 size() const;

protected:
    explicit QTrashFileInfo(const QTrashFileInfoData &dd);

private:
    QExplicitlySharedDataPointer<QTrashFileInfoData> d;

    friend class QTrash;
    friend class QTrashPrivate;
};

typedef QList<QTrashFileInfo> QTrashFileInfoList;

#endif // QTRASHFILEINFO_H
