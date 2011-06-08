#include "history.h"

#include "historyitem.h"

class HistoryPrivate
{
public:
    QList<HistoryItem> items;
    int maximumItemCount;
    int currentItemIndex;
};

History::History(QObject *parent) :
        QObject(parent),
        d_ptr(new HistoryPrivate)
{
    Q_D(History);

    d->maximumItemCount = 0;
    d->currentItemIndex = -1;
}

History::~History()
{
    delete d_ptr;
}

void History::appendItem(const HistoryItem &item)
{
    Q_D(History);

    d->items.erase(d->items.begin() + d->currentItemIndex + 1, d->items.end());
    d->items.append(item);
    d->currentItemIndex++;

    if (d->maximumItemCount != -1 && d->currentItemIndex == d->maximumItemCount) {
        d->currentItemIndex--;
        d->items.takeFirst();
    }
}

bool History::canGoBack() const
{
    Q_D(const History);

    return d->currentItemIndex > 0;
}

void History::back()
{
    Q_D(History);

    if (canGoBack()) {
        d->currentItemIndex--;
        emit currentItemIndexChanged(d->currentItemIndex);
    }
}

bool History::canGoForward() const
{
    Q_D(const History);

    return d->currentItemIndex >= 0 && d->currentItemIndex < d->items.size() - 1;
}

void History::forward()
{
    Q_D(History);

    if (canGoForward()) {
        d->currentItemIndex++;
        emit currentItemIndexChanged(d->currentItemIndex);
    }
}

HistoryItem History::backItem() const
{
    Q_D(const History);

    if (canGoBack()) {
        return d->items.at(d->currentItemIndex - 1);
    } else {
        return HistoryItem();
    }
}

QList<HistoryItem> History::backItems(int maxItems) const
{
    Q_D(const History);

    int pos = d->currentItemIndex - maxItems;

    if (pos < 0 || maxItems == -1) {
        pos = 0;
        maxItems = d->currentItemIndex;
    }

    return d->items.mid(pos, maxItems);
}

// TODO: clear from..to?
void History::clear()
{
    Q_D(History);

    d->items.clear();
    d->currentItemIndex = -1;
}

int History::count() const
{
    return d_func()->items.size();
}

HistoryItem History::currentItem() const
{
    Q_D(const History);

    if (d->currentItemIndex != -1) {
        return d->items.at(d->currentItemIndex);
    } else {
        return HistoryItem();
    }
}

int History::currentItemIndex() const
{
    return d_func()->currentItemIndex;
}

void History::setCurrentItemIndex(int index)
{
    Q_D(History);

    if (index < 0 || index >= count()) {
        return;
    }

    if (d->currentItemIndex != index) {
        d->currentItemIndex = index;
        emit currentItemIndexChanged(index);
    }
}

HistoryItem History::forwardItem() const
{
    Q_D(const History);

    if (canGoForward())
        return d->items.at(d->currentItemIndex + 1);

    return HistoryItem();
}

QList<HistoryItem> History::forwardItems(int maxItems) const
{
    Q_D(const History);

    return d->items.mid(d->currentItemIndex, maxItems);
}

void History::goToItem(const HistoryItem & item)
{
    Q_D(History);

    int index = d->items.indexOf(item);
    setCurrentItemIndex(index);
}

HistoryItem History::itemAt(int i) const
{
    Q_D(const History);

    if (i >= 0 && i < d->items.size())
        return d->items.at(i);
    else
        return HistoryItem();
}

QList<HistoryItem> History::items() const
{
    return d_func()->items;
}

int History::maximumItemCount() const
{
    return d_func()->maximumItemCount;
}

void History::setMaximumItemCount(int count)
{
    d_func()->maximumItemCount = count;
}
