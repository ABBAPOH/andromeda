#include "stackedhistory.h"
#include "stackedhistory_p.h"

#include <QtCore/QDebug>

using namespace GuiSystem;

StackedHistoryPrivate::StackedHistoryPrivate(StackedHistory *qq) :
    m_container(0),
    currentIndex(-1),
    currentStashedIndex(-1),
    currentLocalIndex(0),
    blockHistrory(false),
    q(qq)
{
}

StackedHistory::StackedHistory(QObject *parent) :
    IHistory(parent),
    d(new StackedHistoryPrivate(this))
{
}

StackedHistory::~StackedHistory()
{
    delete d;
}

void StackedHistory::setContainer(GuiSystem::StackedContainer *container)
{
    d->m_container = container;
}

void StackedHistory::open(const QUrl &url)
{
    Q_ASSERT(d->m_container);

    int newEditorIndex = d->m_container->currentIndex();
    AbstractEditor *editor = d->m_container->editor(newEditorIndex);
    if (!editor)
        return;

    // erase forward history
    if (d->items.count() >= d->currentIndex + 1)
        d->items.erase(d->items.begin() + d->currentIndex + 1, d->items.end());

    if (d->currentEditor != editor->id()) {
        // erase forward stashed history
        d->stashedHistory.erase(d->stashedHistory.begin() + d->currentStashedIndex + 1, d->stashedHistory.end());
        d->stashedHistory.append(QByteArray());

        d->currentLocalIndex = -1;
        if (!d->currentEditor.isEmpty()) {
            AbstractEditor *oldEditor = d->m_container->editor(d->currentEditor);
            d->stashEditor(oldEditor);
        }
        d->currentEditor = editor->id();
        d->currentStashedIndex++;

        IHistory *history = editor->history();
        if (history) {
    #ifdef STACKED_HISTORY_DEBUG
            qDebug() << "  connected to editor with history";
    #endif
            // connects only to current data. TODO: watch all datas?
            connect(history, SIGNAL(currentItemIndexChanged(int)), this, SLOT(localHistoryIndexChanged(int)));
        } else {
    #ifdef STACKED_HISTORY_DEBUG
            qDebug() << "  connected to editor without history";
    #endif
            connect(editor, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
        }
    }

    d->currentLocalIndex++;
    StackedHistoryItem item;
    item.url = url;
    item.editor = d->currentEditor;
    item.localIndex = d->currentLocalIndex;
    item.stashedIndex = d->currentStashedIndex;
    d->currentIndex++;
    d->items.append(item);

#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistory::open" << index;
    qDebug() << "  currentIndex" << d->currentIndex;
    qDebug() << "  currentEditor" << d->currentEditor;
    qDebug() << "  currentStashedIndex" << d->currentStashedIndex;
    qDebug() << "  currentLocalIndex" << d->currentLocalIndex;
    qDebug() << "  urls" << d->urls();
    qDebug();
#endif

    emit currentItemIndexChanged(d->currentIndex);
}

int StackedHistory::count() const
{
    return d->items.count();
}

int StackedHistory::currentItemIndex() const
{
    return d->currentIndex;
}

void StackedHistory::setCurrentItemIndex(int index)
{
    if (d->currentIndex == index)
        return;

    if (index < 0 || index >= count())
        return;

    d->blockHistrory = true;

    StackedHistoryItem &item = d->items[index];

    AbstractEditor *oldEditor = d->m_container->editor(d->currentEditor);
    if (d->currentEditor != item.editor || d->currentStashedIndex != item.stashedIndex) {
        d->stashEditor(oldEditor);

        if (!d->m_container->setEditor(item.editor)) {
            qWarning() << "Can't open editor with id" << item.editor;
            return/* false*/;
        }

        d->currentEditor = item.editor;
        d->currentStashedIndex = item.stashedIndex;

        AbstractEditor *newEditor = d->m_container->editor(item.editor);
        d->unstashEditor(newEditor);
    }

    AbstractEditor *newEditor = d->m_container->editor(item.editor);
    d->currentIndex = index;
    d->currentLocalIndex = item.localIndex;

    IHistory *history = newEditor->history();
    if (history) {
        Q_ASSERT(item.localIndex >= 0);
        Q_ASSERT(item.localIndex < history->count());

        history->setCurrentItemIndex(item.localIndex);
    } else {
        // TODO: stash editor state to current item
        newEditor->open(item.url);
    }

#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistory::setCurrentItemIndex" << index;
    qDebug() << "  currentIndex" << d->currentIndex;
    qDebug() << "  currentEditor" << d->currentEditor;
    qDebug() << "  currentStashedIndex" << d->currentStashedIndex;
    qDebug() << "  currentLocalIndex" << d->currentLocalIndex;
    qDebug() << "  urls" << d->urls();
    qDebug();
#endif

    d->blockHistrory = false;

    emit currentItemIndexChanged(d->currentIndex);
}

HistoryItem StackedHistory::itemAt(int index) const
{
    if (index < 0 || index >= count())
        return HistoryItem();

    HistoryItem item;
    item.setUrl(d->items[index].url);
    return item;
}

QByteArray StackedHistory::store() const
{
    return QByteArray();
}

void StackedHistory::restore(const QByteArray &)
{
}

void StackedHistory::clear()
{
    // TODO: implement
}

void StackedHistoryPrivate::stashEditor(AbstractEditor *editor)
{
#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistoryPrivate::stashEditor" << editor->id() << currentStashedIndex << stashedHistory.count();
#endif

    IHistory *history = editor->history();
    editor->cancel();

    QByteArray stored;
    if (history) {
        QObject::disconnect(history, SIGNAL(currentItemIndexChanged(int)), q, SLOT(localHistoryIndexChanged(int)));

        stored = history->store();
    } else {
        QObject::disconnect(editor, SIGNAL(urlChanged(QUrl)), q, SLOT(onUrlChanged(QUrl)));

        stored = editor->saveState();
    }

    stashedHistory[currentStashedIndex] = stored;
    editor->clear();
}

void StackedHistoryPrivate::unstashEditor(AbstractEditor *editor)
{
#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistoryPrivate::unstashEditor" << editor->id() << currentStashedIndex << stashedHistory.count();
#endif

    IHistory *history = editor->history();

    QByteArray stored = stashedHistory[currentStashedIndex];
    Q_ASSERT(!stored.isEmpty());

    if (history) {
        QObject::connect(history, SIGNAL(currentItemIndexChanged(int)), q, SLOT(localHistoryIndexChanged(int)));
        history->restore(stored);
    } else {
        QObject::connect(editor, SIGNAL(urlChanged(QUrl)), q, SLOT(onUrlChanged(QUrl)));
        editor->restoreState(stored);
    }
}

void StackedHistory::localHistoryIndexChanged(int index)
{
    if (d->blockHistrory) {
        // we've came here as a result of setCurrentIndex
        return;
    }

    // otherwise it is an action triggered by user or sript within editor

    IHistory *history = qobject_cast<IHistory *>(sender());
    Q_ASSERT(history);

    int localCount = history->count();
    int start = d->currentIndex - d->currentLocalIndex;

    if (d->items.count() > start + localCount)
        d->items.erase(d->items.begin() + start + localCount, d->items.end());

    if (d->stashedHistory.count() >= d->currentStashedIndex + 1)
        d->stashedHistory.erase(d->stashedHistory.begin() + d->currentStashedIndex + 1, d->stashedHistory.end());

    d->currentIndex = start + index;
    d->currentLocalIndex = index;

    StackedHistoryItem item;
    item.url = history->itemAt(index).url();
    item.editor = d->currentEditor;
    item.localIndex = index;
    item.stashedIndex = d->currentStashedIndex;

    if (start + index == d->items.count())
        d->items.append(item);
    else
        d->items[start + index] = item;

#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistory::localHistoryIndexChanged" << index;
    qDebug() << "  items.count" << d->items.count();
    qDebug() << "  currentIndex" << d->currentIndex;
    qDebug() << "  currentEditor" << d->currentEditor;
    qDebug() << "  currentStashedIndex" << d->currentStashedIndex;
    qDebug() << "  currentLocalIndex" << d->currentLocalIndex;
    qDebug() << "  urls" << d->urls();
    qDebug();
#endif

    emit currentItemIndexChanged(d->currentIndex);
}

void StackedHistory::onUrlChanged(const QUrl &url)
{
#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistory::onUrlChanged" << url;
#endif
    d->items[d->currentIndex].url = url;
}
