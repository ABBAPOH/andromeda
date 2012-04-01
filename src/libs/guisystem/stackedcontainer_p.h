#ifndef STACKEDCONTAINER_P_H
#define STACKEDCONTAINER_P_H

#include "stackedcontainer.h"

#include "abstracteditorfactory.h"
#include "ihistory.h"
#include "editormanager.h"
#include "history.h"
#include "proxyfile.h"

class QStackedLayout;

namespace GuiSystem {

class StackedEditorHistory : public IHistory
{
    Q_OBJECT

public:
    explicit StackedEditorHistory(QObject *parent) :
        IHistory(parent),
        m_history(new History(this))
    {
        connect(m_history, SIGNAL(currentItemIndexChanged(int)),
                SIGNAL(currentItemIndexChanged(int)));

        connect(m_history, SIGNAL(canGoBackChanged(bool)), SIGNAL(canGoBackChanged(bool)));
        connect(m_history, SIGNAL(canGoForwardChanged(bool)), SIGNAL(canGoForwardChanged(bool)));
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

class StackedContainerPrivate
{
    Q_DECLARE_PUBLIC(StackedContainer)
public:
    StackedContainerPrivate(StackedContainer *qq) : q_ptr(qq) {}

    QStackedLayout *layout;
    QUrl currentUrl;
    AbstractEditor *editor;
    QHash<QString, AbstractEditor *> editorHash;
    ProxyFile *file;
    StackedEditorHistory * history;
    bool ignoreSignals;

public:
    void setEditor(AbstractEditor *e);
    void addItem(AbstractEditor *e);

protected:
    StackedContainer *q_ptr;
};

} // namespace GuiSystem

#endif // STACKEDCONTAINER_P_H
