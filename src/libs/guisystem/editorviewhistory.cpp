#include "editorviewhistory.h"
#include "editorviewhistory_p.h"

#include <QtCore/QDebug>

#include "abstractdocument.h"

using namespace GuiSystem;

EditorViewHistoryPrivate::EditorViewHistoryPrivate(EditorViewHistory *qq) :
    m_container(0),
    currentIndex(-1),
    currentStashedIndex(-1),
    currentLocalIndex(0),
    blockHistrory(false),
    q(qq)
{
}

EditorViewHistory::EditorViewHistory(QObject *parent) :
    IHistory(parent),
    d(new EditorViewHistoryPrivate(this))
{
}

EditorViewHistory::~EditorViewHistory()
{
    delete d;
}

void EditorViewHistory::setContainer(GuiSystem::EditorView *container)
{
    d->m_container = container;
}

void EditorViewHistory::open(const QUrl &url, AbstractEditor *oldEditor)
{
    Q_ASSERT(d->m_container);

//    int newEditorIndex = d->m_container->currentIndex();
    AbstractEditor *editor = d->m_container->currentEditor();
    if (!editor)
        return;

    // erase forward history
    if (d->items.count() >= d->currentIndex + 1)
        d->items.erase(d->items.begin() + d->currentIndex + 1, d->items.end());

    QByteArray id = editor->property("id").toByteArray();
    Q_ASSERT_X(!id.isEmpty(), "EditorViewHistory::open",
               "Editor has no id, that may mean it was created without factory");

    if (d->currentEditor != id) {
        // erase forward stashed history
        d->stashedHistory.erase(d->stashedHistory.begin() + d->currentStashedIndex + 1, d->stashedHistory.end());
        d->stashedHistory.append(QByteArray());

        d->currentLocalIndex = -1;
        if (oldEditor) {
            d->stashEditor(oldEditor);
        }
        d->currentEditor = id;
        d->currentStashedIndex++;

        IHistory *history = editor->document()->history();
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
            connect(editor->document(), SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
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

int EditorViewHistory::count() const
{
    return d->items.count();
}

int EditorViewHistory::currentItemIndex() const
{
    return d->currentIndex;
}

void EditorViewHistory::setCurrentItemIndex(int index)
{
    if (d->currentIndex == index)
        return;

    if (index < 0 || index >= count())
        return;

    d->blockHistrory = true;

    StackedHistoryItem &item = d->items[index];

    AbstractEditor *oldEditor = d->m_container->editor();
    if (d->currentEditor != item.editor || d->currentStashedIndex != item.stashedIndex) {
        d->stashEditor(oldEditor);

        d->m_container->openEditor(item.editor);

        d->currentEditor = item.editor;
        d->currentStashedIndex = item.stashedIndex;

        AbstractEditor *newEditor = d->m_container->editor();
        d->unstashEditor(newEditor);
    }

    AbstractEditor *newEditor = d->m_container->editor();
    d->currentIndex = index;
    d->currentLocalIndex = item.localIndex;

    IHistory *history = newEditor->document()->history();
    if (history) {
        Q_ASSERT(item.localIndex >= 0);
        Q_ASSERT(item.localIndex < history->count());

        history->setCurrentItemIndex(item.localIndex);
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

HistoryItem EditorViewHistory::itemAt(int index) const
{
    if (index < 0 || index >= count())
        return HistoryItem();

    HistoryItem item;
    item.setUrl(d->items[index].url);
    item.setUserData(QLatin1String("editor"), d->items[index].editor);
    return item;
}

QByteArray EditorViewHistory::store() const
{
    return QByteArray();
}

void EditorViewHistory::restore(const QByteArray &)
{
}

void EditorViewHistory::clear()
{
    // TODO: implement
}

void EditorViewHistoryPrivate::stashEditor(AbstractEditor *editor)
{
#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistoryPrivate::stashEditor" << editor->id() << currentStashedIndex << stashedHistory.count();
#endif

    IHistory *history = editor->document()->history();
    editor->document()->stop();

    QByteArray stored;
    if (history) {
        QObject::disconnect(history, SIGNAL(currentItemIndexChanged(int)), q, SLOT(localHistoryIndexChanged(int)));

        stored = history->store();
    } else {
        QObject::disconnect(editor->document(), SIGNAL(urlChanged(QUrl)), q, SLOT(onUrlChanged(QUrl)));

        stored = editor->saveState();
    }
    Q_ASSERT(!stored.isEmpty());

    stashedHistory[currentStashedIndex] = stored;
    editor->document()->clear();
}

void EditorViewHistoryPrivate::unstashEditor(AbstractEditor *editor)
{
#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistoryPrivate::unstashEditor" << editor->id() << currentStashedIndex << stashedHistory.count();
#endif

    IHistory *history = editor->document()->history();

    QByteArray stored = stashedHistory[currentStashedIndex];
    Q_ASSERT(!stored.isEmpty());

    if (history) {
        QObject::connect(history, SIGNAL(currentItemIndexChanged(int)), q, SLOT(localHistoryIndexChanged(int)));
        history->restore(stored);
    } else {
        QObject::connect(editor->document(), SIGNAL(urlChanged(QUrl)), q, SLOT(onUrlChanged(QUrl)));
        editor->restoreState(stored);
    }
}

void EditorViewHistory::localHistoryIndexChanged(int index)
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

void EditorViewHistory::onUrlChanged(const QUrl &url)
{
#ifdef STACKED_HISTORY_DEBUG
    qDebug() << "StackedHistory::onUrlChanged" << url;
#endif
    d->items[d->currentIndex].url = url;
}
