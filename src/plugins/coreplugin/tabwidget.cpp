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

bool MyTabWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o != tabBar())
        return false;

    if (e->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *me = static_cast<QMouseEvent *>(e);

        if (tabBar()->tabAt(me->pos()) == -1)
            emit tabBarDoubleClicked();

        return true;
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

