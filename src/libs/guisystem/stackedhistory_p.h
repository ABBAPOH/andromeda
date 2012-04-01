#ifndef STACKEDHISTORY_P_H
#define STACKEDHISTORY_P_H

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
    QString editor;
    int localIndex;
    int stashedIndex;
};

class StackedHistoryPrivate
{
public:
    explicit StackedHistoryPrivate(StackedHistory *qq);

    void stashEditor(AbstractEditor *editor);
    void unstashEditor(AbstractEditor *editor);

public:
    StackedContainer *m_container;

    QList<StackedHistoryItem> items;
    int currentIndex;

    QString currentEditor;

    QList<QByteArray> stashedHistory;
    int currentStashedIndex;

    int currentLocalIndex;
    bool blockHistrory;

private:
    StackedHistory *q;
};

} // namespace GuiSystem

#endif // STACKEDHISTORY_P_H
