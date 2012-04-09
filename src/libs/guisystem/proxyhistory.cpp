#include "proxyhistory.h"

using namespace GuiSystem;

/*!
  \class ProxyHistory

  \brief This class redirects method calls and signals to and from other history.
*/
ProxyHistory::ProxyHistory(QObject *parent) :
    IHistory(parent),
    m_sourceHistory(0)
{
}

/*!
  \reimp
*/
int ProxyHistory::count() const
{
    if (m_sourceHistory)
        return m_sourceHistory->count();

    return 0;
}

/*!
  \reimp
*/
int ProxyHistory::currentItemIndex() const
{
    if (m_sourceHistory)
        return m_sourceHistory->currentItemIndex();

    return -1;
}

/*!
  \reimp
*/
void ProxyHistory::setCurrentItemIndex(int index)
{
    if (m_sourceHistory)
        m_sourceHistory->setCurrentItemIndex(index);
}

/*!
  \reimp
*/
HistoryItem ProxyHistory::itemAt(int index) const
{
    if (m_sourceHistory)
        return m_sourceHistory->itemAt(index);

    return HistoryItem();
}

/*!
  \brief Returns source history.
*/
IHistory * ProxyHistory::sourceHistory() const
{
    return m_sourceHistory;
}

/*!
  \brief Sets source \a history for the proxy.
*/
void ProxyHistory::setSourceHistory(IHistory *history)
{
    if (m_sourceHistory == history)
        return;

    if (m_sourceHistory) {
        disconnect(m_sourceHistory, 0, this, 0);
    }

    m_sourceHistory = history;

    int index = -1;
    if (m_sourceHistory) {
        connect(m_sourceHistory, SIGNAL(currentItemIndexChanged(int)), SIGNAL(currentItemIndexChanged(int)));

        index = m_sourceHistory->currentItemIndex();
    }

    // we need to notify that current history changed, even if index is the same
    emit currentItemIndexChanged(index);
}

/*!
  \reimp
*/
void ProxyHistory::clear()
{
    if (m_sourceHistory)
        m_sourceHistory->clear();
}
