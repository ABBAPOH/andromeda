#include "qtrashfileinfo.h"
#include "qtrashfileinfo_p.h"

#include <QtCore/QFileInfo>

QTrashFileInfo::QTrashFileInfo() :
    d(new QTrashFileInfoData)
{
}

QTrashFileInfo::QTrashFileInfo(const QTrashFileInfoData &dd) :
    d(new QTrashFileInfoData(dd))
{
}

QTrashFileInfo::QTrashFileInfo(const QTrashFileInfo &other) :
    d(other.d)
{
}

QTrashFileInfo &QTrashFileInfo::operator=(const QTrashFileInfo &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

QTrashFileInfo::~QTrashFileInfo()
{
}

QString QTrashFileInfo::name() const
{
    return QFileInfo(d->path).fileName();
}

QString QTrashFileInfo::path() const
{
    return d->path;
}

QString QTrashFileInfo::originalName() const
{
    if (d->originalPath.isEmpty())
        return QFileInfo(d->path).fileName();

    return QFileInfo(d->originalPath).fileName();
}

QString QTrashFileInfo::originalPath() const
{
    return d->originalPath;
}

QDateTime QTrashFileInfo::deletionDateTime() const
{
    return d->deletionDateTime;
}

qint64 QTrashFileInfo::size() const
{
    return d->size;
}
