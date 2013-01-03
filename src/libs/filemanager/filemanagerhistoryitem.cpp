#include "filemanagerhistoryitem.h"
#include "filemanagerhistoryitem_p.h"

#include <QtCore/QSharedData>

using namespace FileManager;

FileManagerHistoryItem::FileManagerHistoryItem() :
    d(new FileManagerHistoryItemData)
{
}

FileManagerHistoryItem::FileManagerHistoryItem(FileManagerHistoryItemData &dd) :
    d(new FileManagerHistoryItemData(dd))
{
}

FileManagerHistoryItem::FileManagerHistoryItem(const FileManagerHistoryItem &other) :
    d(other.d)
{
}

FileManagerHistoryItem &FileManagerHistoryItem::operator=(const FileManagerHistoryItem &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

FileManagerHistoryItem::~FileManagerHistoryItem()
{
}

QIcon FileManagerHistoryItem::icon() const
{
    return d->icon;
}

bool FileManagerHistoryItem::isValid() const
{
    return !d->path.isEmpty();
}

QDateTime FileManagerHistoryItem::lastVisited() const
{
    return d->lastVisited;
}

QString FileManagerHistoryItem::title() const
{
    return d->title;
}

QString FileManagerHistoryItem::path() const
{
    return d->path;
}

bool FileManagerHistoryItem::operator==(const FileManagerHistoryItem &other) const
{
    return d == other.d;
}
