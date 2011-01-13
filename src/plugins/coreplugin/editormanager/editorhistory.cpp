#include "editorhistory.h"

#include "historyitem.h"

using namespace CorePlugin;

namespace CorePlugin {

class EditorHistoryPrivate
{
public:
    QList<HistoryItem> items;
    int maximumItemCount;
    int currentItemIndex;
};

} // namespace CorePlugin

EditorHistory::EditorHistory(QObject *parent) :
        QObject(parent),
        d_ptr(new EditorHistoryPrivate)
{
    Q_D(EditorHistory);
    d->maximumItemCount = 0;
    d->currentItemIndex = -1;
}

EditorHistory::~EditorHistory()
{
    delete d_ptr;
}

void EditorHistory::appendItem(const HistoryItem &item)
{
    Q_D(EditorHistory);
    int count = d->items.size() - d->currentItemIndex;
    while (count-- >= 1) {
        d->items.removeAt(d->currentItemIndex + 1);
    }
    d->items.append(item);
    d->currentItemIndex++;
}

bool EditorHistory::canGoBack() const
{
    return d_func()->items.size() > 0;
}

void EditorHistory::back()
{
    Q_D(EditorHistory);
    if (d->currentItemIndex > 0) {
        d->currentItemIndex--;
        emit currentItemIndexChanged(d->currentItemIndex);
    }
}

bool EditorHistory::canGoForward() const
{
    Q_D(const EditorHistory);
    return d->currentItemIndex >= 0 && d->currentItemIndex < d->items.size() - 1;
}

void EditorHistory::forward()
{
    Q_D(EditorHistory);
    if (canGoForward()) {
        d->currentItemIndex++;
        emit currentItemIndexChanged(d->currentItemIndex);
    }
}

HistoryItem EditorHistory::backItem() const
{
    Q_D(const EditorHistory);

    if (canGoBack()) {
        return d->items.at(d->currentItemIndex - 1);
    } else {
        return HistoryItem();
    }
}

QList<HistoryItem> EditorHistory::backItems(int maxItems) const
{
    return QList<HistoryItem>();
}

void EditorHistory::clear()
{
    Q_D(EditorHistory);
    d->items.clear();
    d->currentItemIndex = -1;
}

int EditorHistory::count() const
{
    return d_func()->items.size();
}

HistoryItem EditorHistory::currentItem() const
{
    Q_D(const EditorHistory);
    if (d->currentItemIndex != -1) {
        return d->items.at(d->currentItemIndex);
    } else {
        return HistoryItem();
    }
}

int EditorHistory::currentItemIndex() const
{
    return d_func()->currentItemIndex;
}

HistoryItem EditorHistory::forwardItem() const
{
    Q_D(const EditorHistory);
    if (canGoForward())
        return d->items.at(d->currentItemIndex + 1);
    return HistoryItem();
}

QList<HistoryItem> EditorHistory::forwardItems(int maxItems) const
{
    Q_D(const EditorHistory);
    return d->items.mid(d->currentItemIndex, maxItems);
}

void EditorHistory::goToItem(const HistoryItem & item)
{
    Q_D(EditorHistory);
    int index = d->items.indexOf(item);
    d->currentItemIndex = index;
}

HistoryItem EditorHistory::itemAt(int i) const
{
    return d_func()->items.at(i);
}

QList<HistoryItem> EditorHistory::items() const
{
    return d_func()->items;
}

int EditorHistory::maximumItemCount() const
{
    return d_func()->maximumItemCount;
}

void EditorHistory::setMaximumItemCount(int count)
{
    d_func()->maximumItemCount = count;
}
