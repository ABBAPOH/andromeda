#include "globalhistory.h"

namespace CorePlugin {

class GlobalHistoryPrivate
{
public:
    QList<GlobalHistoryItem *> items;
    int index;
};

} // namespace CorePlugin

using namespace CorePlugin;

GlobalHistory::GlobalHistory(QObject *parent) :
    QObject(parent),
    d_ptr(new GlobalHistoryPrivate)
{
    Q_D(GlobalHistory);

    d->index = -1;
}

GlobalHistory::~GlobalHistory()
{
    delete d_ptr;
}

void GlobalHistory::appendItem(const HistoryItem &item)
{
    Q_D(GlobalHistory);

    // TODO: fix
    d->items.append(new GlobalHistoryItem(item));
    d->index++;
}

void GlobalHistory::appendView(CorePlugin::IHistoryView *view)
{
    Q_D(GlobalHistory);

    // TODO: fix
    d->items.append(new GlobalHistoryItem(view));
    d->index++;
}

bool GlobalHistory::canGoBack() const
{
    Q_D(const GlobalHistory);

    return d->index > 0;
}

bool GlobalHistory::canGoForward() const
{
    Q_D(const GlobalHistory);

    return d->index >= 0 && d->index < d->items.size() - 1;
}

int GlobalHistory::currentIndex() const
{
    return d_func()->index;
}

GlobalHistoryItem * GlobalHistory::currentItem() const
{
    Q_D(const GlobalHistory);

    return d->items.at(d->index);
}

void GlobalHistory::back()
{
    Q_D(GlobalHistory);

    if (d->index > 0) {
        d->index--;
//        emit currentItemIndexChanged(d->index);
    }
}

void GlobalHistory::forward()
{
    Q_D(GlobalHistory);

    if (canGoForward()) {
        d->index++;
//        emit currentItemIndexChanged(d->index);
    }
}

