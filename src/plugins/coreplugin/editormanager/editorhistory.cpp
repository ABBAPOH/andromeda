#include "editorhistory.h"

#include "historyitem.h"

using namespace Core;

namespace Core {

class EditorHistoryPrivate
{
public:
    QList<HistoryItem> items;
    int maximumItemCount;
    int currentItemIndex;
};

} // namespace Core

EditorHistory::EditorHistory(QObject *parent) :
        QObject(parent),
        d_ptr(new EditorHistoryPrivate)
{
    d_func()->maximumItemCount = 0;
}

EditorHistory::~EditorHistory()
{
    delete d_ptr;
}

bool EditorHistory::canGoBack() const
{
    return d_func()->items.size() > 0;
}

void EditorHistory::back()
{
    Q_D(EditorHistory);
    if (d->currentItemIndex > 0)
        d->currentItemIndex--;
}

bool EditorHistory::canGoForward() const
{
    Q_D(const EditorHistory);
    return d->currentItemIndex < d->items.size();
}

void EditorHistory::forward()
{
    Q_D(EditorHistory);
    if (canGoForward())
        d->currentItemIndex++;
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
