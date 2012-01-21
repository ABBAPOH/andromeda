#include "proxyhistory.h"

using namespace GuiSystem;

/*!
  \class ProxyHistory

  \brief This class redirects method calls and signals to and from other history.
*/
ProxyHistory::ProxyHistory(QObject *parent) :
    AbstractHistory(parent),
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
AbstractHistory * ProxyHistory::sourceHistory() const
{
    return m_sourceHistory;
}

/*!
  \brief Sets source \a history for the proxy.
*/
void ProxyHistory::setSourceHistory(AbstractHistory *history)
{
    if (m_sourceHistory == history)
        return;

    bool canGoBack = false;
    bool canGoForward = false;
    if (m_sourceHistory) {
        disconnect(m_sourceHistory, 0, this, 0);
        canGoBack = m_sourceHistory->canGoBack();
        canGoForward = m_sourceHistory->canGoForward();
    }

    m_sourceHistory = history;

    if (m_sourceHistory) {
        connect(m_sourceHistory, SIGNAL(canGoBackChanged(bool)), SIGNAL(canGoBackChanged(bool)));
        connect(m_sourceHistory, SIGNAL(canGoForwardChanged(bool)), SIGNAL(canGoForwardChanged(bool)));
        connect(m_sourceHistory, SIGNAL(currentItemIndexChanged(int)), SIGNAL(currentItemIndexChanged(int)));
    }

    if (canGoBack != m_sourceHistory->canGoBack())
        emit canGoBackChanged(!canGoBack);

    if (canGoForward != m_sourceHistory->canGoForward())
        emit canGoForwardChanged(!canGoForward);
}

/*!
  \reimp
*/
void ProxyHistory::clear()
{
    if (m_sourceHistory)
        m_sourceHistory->clear();
}
