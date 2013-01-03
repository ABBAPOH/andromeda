#include "filemanagerhistory.h"
#include "filemanagerhistory_p.h"

#include "filemanagerhistoryitem_p.h"

void FileManagerHistoryPrivate::appendItem(const FileManagerHistoryItem &item)
{
    if (!item.isValid())
        return;

    if (items.contains(item))
        return;

    items.erase(items.begin() + currentItemIndex + 1, items.end());
    items.append(item);

    if (maximumItemCount != -1 && currentItemIndex == maximumItemCount) {
        currentItemIndex--;
        items.takeFirst();
    }

    setCurrentItemIndex(currentItemIndex + 1);
}

void FileManagerHistoryPrivate::setCurrentItemIndex(int index)
{
    int oldIndex = currentItemIndex;
    currentItemIndex = index;
    emit q->currentItemIndexChanged(index);

    if (index == 0)
        emit q->canGoBackChanged(false);
    else if (oldIndex == 0)
        emit q->canGoBackChanged(true);
    if (index == q->count() - 1)
        emit q->canGoForwardChanged(false);
    else if (oldIndex == q->count() - 1)
        emit q->canGoForwardChanged(true);
}

FileManagerHistory::FileManagerHistory(QObject *parent) :
    QObject(parent),
    d_ptr(new FileManagerHistoryPrivate(this))
{
    Q_D(FileManagerHistory);

    d->maximumItemCount = -1;
    d->currentItemIndex = -1;
}

FileManagerHistory::~FileManagerHistory()
{
    delete d_ptr;
}

bool FileManagerHistory::canGoBack() const
{
    Q_D(const FileManagerHistory);

    return d->currentItemIndex > 0;
}

void FileManagerHistory::back()
{
    Q_D(FileManagerHistory);

    if (canGoBack()) {
        d->setCurrentItemIndex(d->currentItemIndex - 1);
    }
}

bool FileManagerHistory::canGoForward() const
{
    Q_D(const FileManagerHistory);

    return d->currentItemIndex >= 0 && d->currentItemIndex < d->items.size() - 1;
}

void FileManagerHistory::forward()
{
    Q_D(FileManagerHistory);

    if (canGoForward()) {
        d->setCurrentItemIndex(d->currentItemIndex + 1);
    }
}

FileManagerHistoryItem FileManagerHistory::backItem() const
{
    Q_D(const FileManagerHistory);

    if (canGoBack()) {
        return d->items.at(d->currentItemIndex - 1);
    } else {
        return FileManagerHistoryItem();
    }
}

QList<FileManagerHistoryItem> FileManagerHistory::backItems(int maxItems) const
{
    Q_D(const FileManagerHistory);

    int pos = d->currentItemIndex - maxItems;

    if (pos < 0 || maxItems == -1) {
        pos = 0;
        maxItems = d->currentItemIndex;
    }

    return d->items.mid(pos, maxItems);
}

void FileManagerHistory::clear()
{
    Q_D(FileManagerHistory);

    d->items.clear();
    d->currentItemIndex = -1;
}

int FileManagerHistory::count() const
{
    return d_func()->items.size();
}

FileManagerHistoryItem FileManagerHistory::currentItem() const
{
    Q_D(const FileManagerHistory);

    if (d->currentItemIndex != -1) {
        return d->items.at(d->currentItemIndex);
    } else {
        return FileManagerHistoryItem();
    }
}

int FileManagerHistory::currentItemIndex() const
{
    return d_func()->currentItemIndex;
}

void FileManagerHistory::setCurrentItemIndex(int index)
{
    Q_D(FileManagerHistory);

    if (index < 0 || index >= count()) {
        return;
    }

    if (d->currentItemIndex != index)
        d->setCurrentItemIndex(index);
}

FileManagerHistoryItem FileManagerHistory::forwardItem() const
{
    Q_D(const FileManagerHistory);

    if (canGoForward())
        return d->items.at(d->currentItemIndex + 1);

    return FileManagerHistoryItem();
}

QList<FileManagerHistoryItem> FileManagerHistory::forwardItems(int maxItems) const
{
    Q_D(const FileManagerHistory);

    return d->items.mid(d->currentItemIndex, maxItems);
}

void FileManagerHistory::goToItem(const FileManagerHistoryItem & item)
{
    Q_D(FileManagerHistory);

    int index = d->items.indexOf(item);
    setCurrentItemIndex(index);
}

FileManagerHistoryItem FileManagerHistory::itemAt(int i) const
{
    Q_D(const FileManagerHistory);

    if (i >= 0 && i < d->items.size())
        return d->items.at(i);
    else
        return FileManagerHistoryItem();
}

QList<FileManagerHistoryItem> FileManagerHistory::items() const
{
    return d_func()->items;
}

int FileManagerHistory::maximumItemCount() const
{
    return d_func()->maximumItemCount;
}

void FileManagerHistory::setMaximumItemCount(int count)
{
    d_func()->maximumItemCount = count;
}

QDataStream & operator<<(QDataStream & stream, const FileManagerHistoryItem &item)
{
    stream << item.path();
    stream << item.title();
    stream << item.lastVisited();
    stream << item.icon();

    return stream;
}

QDataStream & operator>>(QDataStream & stream, FileManagerHistoryItem &item)
{
    FileManagerHistoryItemData data;

    stream >> data.path;
    stream >> data.title;
    stream >> data.lastVisited;
    stream >> data.icon;

    item = FileManagerHistoryItem(data);

    return stream;
}

QDataStream & FileManager::operator<<(QDataStream & stream, const FileManagerHistory &history)
{
    QList<FileManagerHistoryItem> items = history.d_func()->items;
    stream << items;

    stream << history.d_func()->currentItemIndex;
    stream << history.d_func()->maximumItemCount;

    return stream;
}

QDataStream & FileManager::operator>>(QDataStream & stream, FileManagerHistory &history)
{
    QList<FileManagerHistoryItem> items;
    stream >> items;
    history.d_func()->items = items;

    int currentIndex;
    stream >> currentIndex;

    stream >> history.d_func()->maximumItemCount;
    history.setCurrentItemIndex(currentIndex);

    return stream;
}
