#include "tabwidget.h"

#include <QtGui/QMouseEvent>

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionTabBarBaseV2>

MyTabWidget::MyTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    TabBar *tabBar = new TabBar;
    tabBar->setSwitchTabsOnDrag(true);
    tabBar->installEventFilter(this);
    setTabBar(tabBar);
}

bool MyTabWidget::isTabBarVisible() const
{
    return tabBar()->isVisible();
}

void MyTabWidget::setTabBarVisible(bool visible)
{
    tabBar()->setVisible(visible);
}

bool MyTabWidget::eventFilter(QObject *o, QEvent *e)
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

void TabBarButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    QStyleOptionTabBarBaseV2 opt;
    opt.init(this);
    // hardcoded document Mode
    opt.documentMode = true;

    int overlap = style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &opt, this);
    QRect rect;
    // hardcoded north position
    rect.setRect(0, size().height() - overlap, size().width(), overlap);
    opt.rect = rect;

    qApp->style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &opt, &p, this);

    int w = iconSize().width();
    int h = iconSize().height();
    QIcon::Mode mode = pressed ? QIcon::Selected : QIcon::Normal;

    icon().paint(&p, (width() - w)/2, (height() - h)/2, w, h, Qt::AlignCenter, mode);
}

QSize TabBarButton::sizeHint() const
{
    return iconSize();
}

