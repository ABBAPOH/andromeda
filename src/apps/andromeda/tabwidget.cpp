#include "tabwidget.h"

#include <QtGui/QMouseEvent>

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    TabBar *tabBar = new TabBar;
    tabBar->setSwitchTabsOnDrag(true);
    tabBar->installEventFilter(this);
    setTabBar(tabBar);
}

bool TabWidget::isTabBarVisible() const
{
    return tabBar()->isVisible();
}

void TabWidget::setTabBarVisible(bool visible)
{
    tabBar()->setVisible(visible);
}

bool TabWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o != tabBar())
        return false;

    switch (e->type()) {
    case QEvent::MouseButtonDblClick : {
        QMouseEvent *me = static_cast<QMouseEvent *>(e);

        if (tabBar()->tabAt(me->pos()) == -1)
            emit tabBarDoubleClicked();

        return true;
    }
    case QEvent::Hide :
    case QEvent::Show : {
        emit tabBarVisibleChanged(tabBar()->isVisible());
    }
    default : break;
    }
    return false;
}
