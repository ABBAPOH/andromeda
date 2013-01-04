#ifndef STACKEDHISTORY_P_H
#define STACKEDHISTORY_P_H

//#define STACKED_HISTORY_DEBUG

#include "stackedhistory.h"
#include "stackedcontainer.h"

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

class StackedHistoryPrivate
{
public:
    explicit StackedHistoryPrivate(StackedHistory *qq);

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
    StackedContainer *m_container;

    QList<StackedHistoryItem> items;
    int currentIndex;

    QByteArray currentEditor;

    QList<QByteArray> stashedHistory;
    int currentStashedIndex;

    int currentLocalIndex;
    bool blockHistrory;

private:
    StackedHistory *q;
};

} // namespace GuiSystem

#endif // STACKEDHISTORY_P_H
