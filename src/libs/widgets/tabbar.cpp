#include "tabbar.h"

#include <QDragEnterEvent>
#include <QTimer>

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent),
    m_timer(new QTimer(this))
{
    m_tabIndex = -1;
    m_switchTabsOnDrag = false;
    m_switchOnHover = false;
    m_switchTimeout = 500;
    setElideMode(Qt::ElideRight);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(changeTab()));
}

/*!
    \property TabBar::switchTabsOnDrag
    \brief If set to true, switches current tab while user drags something over the tab

    By default, this property is false;
*/
bool TabBar::switchTabsOnDrag()
{
    return m_switchTabsOnDrag;
}

void TabBar::setSwitchTabsOnDrag(bool switchTabsOnDrag)
{
    if (m_switchTabsOnDrag == switchTabsOnDrag)
        return;
    setAcceptDrops(switchTabsOnDrag);
    m_switchTabsOnDrag = switchTabsOnDrag;
}

/*!
    \property TabBar::switchOnHover
    \brief If set to true, tab is switched moving mouse over it, otherwise
    it is switched after mouse being stopped.

    By default, this property is false;
*/
bool TabBar::switchOnHover()
{
    return m_switchOnHover;
}

void TabBar::setSwitchOnHover(bool switchOnHover)
{
    m_switchOnHover = switchOnHover;
}

/*!
    \property TabBar::switchTimeout
    \brief Timeout before tab is switched while performing drag'n'drop operation

    Default value is 500 milliseconds.
*/
int TabBar::switchTimeout()
{
    return m_switchTimeout;
}

void TabBar::setSwitchTimeout(int milliseconds)
{
    m_switchTimeout = milliseconds;
}

void TabBar::changeTab()
{
    setCurrentIndex(m_tabIndex);
    m_timer->stop();
}

/*!
    \reimp
*/
void TabBar::dragEnterEvent(QDragEnterEvent *event)
{
    if (m_switchTabsOnDrag)
        event->accept();
    else
        event->ignore();
}

/*!
    \reimp
*/
void TabBar::dragMoveEvent(QDragMoveEvent *event)
{
    event->ignore();
    int tabIndex = tabAt(event->pos());
    if (!m_switchOnHover || m_tabIndex != tabIndex) {
        m_tabIndex = tabIndex;
        m_timer->stop();
        m_timer->start(m_switchTimeout);
    }
}

/*!
    \reimp
*/
void TabBar::dragLeaveEvent(QDragLeaveEvent * event)
{
    m_timer->stop();
}

/*!
    \reimp
*/
QSize TabBar::tabSizeHint(int index) const
{
    QSize s = QTabBar::tabSizeHint(index);
    return QSize(qBound(100, width()/count(), 200), s.height());
}
