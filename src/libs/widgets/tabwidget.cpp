#include "tabwidget.h"

#include <QtGui/QMouseEvent>

#include "tabbar.h"

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

bool TabWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object != tabBar())
        return false;

    switch (event->type()) {
    case QEvent::MouseButtonDblClick : {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);

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
