#include "tabbarbutton.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionTabBarBaseV2>

TabBarButton::TabBarButton(QWidget *parent) :
    QAbstractButton(parent),
    hovered(false),
    pressed(false)
{
}

QSize TabBarButton::sizeHint() const
{
    return iconSize();
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

void TabBarButton::enterEvent(QEvent *)
{
    hovered = true;
    update();
}

void TabBarButton::leaveEvent(QEvent *)
{
    hovered = false;
    update();
}

void TabBarButton::mousePressEvent(QMouseEvent *e)
{
    pressed = true;
    update();
    QAbstractButton::mousePressEvent(e);
}

void TabBarButton::mouseReleaseEvent(QMouseEvent *e)
{
    pressed = false;
    update();
    QAbstractButton::mouseReleaseEvent(e);
}
