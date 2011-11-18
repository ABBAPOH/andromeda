#ifndef STACKEDEDITOR_P_H
#define STACKEDEDITOR_P_H

#include "stackededitor.h"

#include "abstracteditorfactory.h"
#include "abstracthistory.h"
#include "editormanager.h"
#include "history.h"

class QStackedLayout;

namespace GuiSystem {

class StackedEditorHistory : public AbstractHistory
{
    Q_OBJECT

public:
    explicit StackedEditorHistory(QObject *parent) :
        AbstractHistory(parent),
        m_history(new History(this))
    {
        connect(m_history, SIGNAL(currentItemIndexChanged(int)),
                SIGNAL(currentItemIndexChanged(int)));
    }

    History *history() const { return m_history; }

    virtual int count() const { return m_history->count(); }

    virtual int currentItemIndex() const { return m_history->currentItemIndex(); }
    virtual void setCurrentItemIndex(int index) { m_history->setCurrentItemIndex(index); }

    virtual HistoryItem itemAt(int index) const { return m_history->itemAt(index); }
    virtual void clear() { m_history->clear(); }

private:
    History *m_history;
};

class StackedEditorPrivate
{
    Q_DECLARE_PUBLIC(StackedEditor)
public:
    StackedEditorPrivate(StackedEditor *qq) : q_ptr(qq) {}

    QStackedLayout *layout;
    QUrl currentUrl;
    AbstractEditor *editor;
    QHash<QString, AbstractEditor *> editorHash;
    StackedEditorHistory * history;
    bool ignoreSignals;

public:
    void setEditor(AbstractEditor *e);
    void addItem(AbstractEditor *e);

protected:
    StackedEditor *q_ptr;
};

} // namespace GuiSystem

#endif // STACKEDEDITOR_P_H
