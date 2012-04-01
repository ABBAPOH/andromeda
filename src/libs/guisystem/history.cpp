#include "history.h"

#include "historyitem.h"

namespace GuiSystem {

class HistoryPrivate
{
public:
    HistoryPrivate(History *qq) : q(qq) {}

public:
    IHistory *history;

    int currentItemIndex;
    bool canGoBack;
    bool canGoForward;

private:
    History *q;
};

} // namespace GuiSystem

using namespace GuiSystem;

History::History(QObject *parent) :
    QObject(parent),
    d_ptr(new HistoryPrivate(this))
{
    Q_D(History);

    d->canGoBack = false;
    d->canGoForward = false;

    d->history = 0;
    d->currentItemIndex = -1;
}

History::~History()
{
    delete d_ptr;
}

void History::setHistory(IHistory *history)
{
    Q_D(History);

    if (d->history == history)
        return;

    d->history = history;

    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onCurrentItemIndexChanged(int)));
    onCurrentItemIndexChanged(d->history->currentItemIndex());
}

bool History::canGoBack() const
{
    Q_D(const History);

    return d->canGoBack;
}

void History::back()
{
    Q_D(History);

    if (canGoBack()) {
        setCurrentItemIndex(d->currentItemIndex - 1);
    }
}

bool History::canGoForward() const
{
    Q_D(const History);

    return d->canGoForward;
}

void History::forward()
{
    Q_D(History);

    if (canGoForward()) {
        setCurrentItemIndex(d->currentItemIndex + 1);
    }
}

HistoryItem History::backItem() const
{
    Q_D(const History);

    if (canGoBack()) {
        return d->history->itemAt(d->currentItemIndex - 1);
    } else {
        return HistoryItem();
    }
}

QList<HistoryItem> History::backItems(int maxItems) const
{
    Q_D(const History);

    QList<HistoryItem> items;

    if (!d->history)
        return items;

    if (maxItems < 0)
        maxItems = d->currentItemIndex;

    maxItems = qMin(maxItems, d->currentItemIndex);

    for (int i = d->currentItemIndex - maxItems; i < d->currentItemIndex; i++) {
        items.append(d->history->itemAt(i));
    }

    return items;
}

// TODO: clear from..to?
void History::clear()
{
    Q_D(History);

    d->history->clear();
    d->currentItemIndex = 0;
}

int History::count() const
{
    Q_D(const History);

    if (d->history)
        return d->history->count();

    return 0;
}

HistoryItem History::currentItem() const
{
    Q_D(const History);

    if (d->currentItemIndex != -1) {
        if (d->history)
            return d->history->itemAt(d->currentItemIndex);
    }

    return HistoryItem();
}

int History::currentItemIndex() const
{
    Q_D(const History);

    return d->currentItemIndex;
}

void History::setCurrentItemIndex(int index)
{
    Q_D(History);

    if (d->history)
        d->history->setCurrentItemIndex(index);
}

HistoryItem History::forwardItem() const
{
    Q_D(const History);

    if (canGoForward())
        if (d->history)
            return d->history->itemAt(d->currentItemIndex + 1);

    return HistoryItem();
}

QList<HistoryItem> History::forwardItems(int maxItems) const
{
    Q_D(const History);

    QList<HistoryItem> items;

    if (!d->history)
        return items;

    int count = d->history->count();
    if (maxItems < 0)
        maxItems = count - d->currentItemIndex;

    count = qMin(d->currentItemIndex + maxItems, count);

    for (int i = d->currentItemIndex + 1; i < count; i++) {
        items.append(d->history->itemAt(i));
    }

    return items;
}

HistoryItem History::itemAt(int index) const
{
    Q_D(const History);

    if (d->history)
        return d->history->itemAt(index);

    return HistoryItem();
}

QList<HistoryItem> History::items() const
{
    Q_D(const History);

    QList<HistoryItem> result;
    for (int i = 0; i < d->history->count(); i++) {
        result.append(d->history->itemAt(i));
    }

    return result;
}

void History::onCurrentItemIndexChanged(int index)
{
    Q_D(History);

    if (d->currentItemIndex == index)
        return;

    d->currentItemIndex = index;

    bool canGoBack = d->canGoBack;
    bool canGoForward = d->canGoForward;
    d->canGoBack = index > 0;
    d->canGoForward = index < d->history->count() - 1;

    emit currentItemIndexChanged(d->currentItemIndex);

    if (canGoBack != d->canGoBack)
        emit canGoBackChanged(d->canGoBack);

    if (canGoForward != d->canGoForward)
        emit canGoForwardChanged(d->canGoForward);
}
