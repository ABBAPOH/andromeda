#ifndef EDITORVIEWHISTORY_P_H
#define EDITORVIEWHISTORY_P_H

//#define STACKED_HISTORY_DEBUG

#include "editorviewhistory.h"
#include "editorview.h"

namespace GuiSystem {

struct StackedHistoryItem
{
    StackedHistoryItem() :
        localIndex(-1),
        stashedIndex(0)
    {
    }

    QUrl url;
    QByteArray editor;
    int localIndex;
    int stashedIndex;
};

class EditorViewHistoryPrivate
{
public:
    explicit EditorViewHistoryPrivate(EditorViewHistory *qq);

    void stashEditor(AbstractEditor *editor);
    void unstashEditor(AbstractEditor *editor);

#ifdef STACKED_HISTORY_DEBUG
    QList<QUrl> urls() const
    {
        QList<QUrl> res;
        for (int i = 0; i < items.count(); ++i) {
            res.append(items[i].url);
        }
        return res;
    }
#endif

public:
    EditorView *m_container;

    QList<StackedHistoryItem> items;
    int currentIndex;

    QByteArray currentEditor;

    QList<QByteArray> stashedHistory;
    int currentStashedIndex;

    int currentLocalIndex;
    bool blockHistrory;

private:
    EditorViewHistory *q;
};

} // namespace GuiSystem

#endif // EDITORVIEWHISTORY_P_H
