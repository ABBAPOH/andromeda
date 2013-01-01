#include "tabbarbutton.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionTabBarBaseV2>

static inline QRect buttonRect(const QWidget *widget, const QStyleOption &option)
{
    if ( const QStyleOptionTabBarBase *tbo = qstyleoption_cast<const QStyleOptionTabBarBase *>(&option) ) {
        int overlap = widget->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &option, widget);
        QRect rect = tbo->rect;
        switch (tbo->shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            return QRect(QPoint(0, qMax(rect.height() - overlap, 0)),
                         QSize(rect.width(), qMin(rect.height() - rect.height() + overlap, rect.height())));
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            return QRect(QPoint(0,0),
                         QSize(rect.width(), qMin(rect.height() - rect.height() + overlap, rect.height())));
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            return QRect(QPoint(0, 0),
                      QSize(qMin(rect.width() - rect.width() + overlap, rect.width()), rect.height()));
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            return QRect(QPoint(qMax(rect.width() - overlap, 0), 0),
                      QSize(qMin(rect.width() - rect.width() + overlap, rect.width()), rect.height()));
        }
    }
    return option.rect;
}

static void initStyleOption(const TabBarButton *widget, QStyleOption &option)
{
    Q_ASSERT(widget);

    option.initFrom(widget);
    if (option.state & QStyle::State_MouseOver) {
        if (qApp->mouseButtons())
            option.state |= QStyle::State_Sunken;
    }

    if (QStyleOptionTabBarBase *optionBase = qstyleoption_cast<QStyleOptionTabBarBase *>(&option)) {
        optionBase->shape = widget->shape();
    }

    if (QStyleOptionTabBarBaseV2 *optionV2 = qstyleoption_cast<QStyleOptionTabBarBaseV2 *>(&option)) {
        // hardcoded document Mode
        optionV2->documentMode = true;
    }

    option.rect = buttonRect(widget, option);
}

TabBarButton::TabBarButton(QWidget *parent) :
    QAbstractButton(parent),
    m_shape(QTabBar::RoundedNorth)
{
}

QTabBar::Shape TabBarButton::shape() const
{
    return m_shape;
}

void TabBarButton::setShape(QTabBar::Shape shape)
{
    if (m_shape == shape)
        return;

    m_shape = shape;
    update();
}

QSize TabBarButton::sizeHint() const
{
    return iconSize();
}

bool TabBarButton::event(QEvent *event)
{
    bool ok = QWidget::event(event);
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::Leave:
        update();
        break;
    default:
        break;
    }
    return ok;
}

void TabBarButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    QStyleOptionTabBarBaseV2 option;
    initStyleOption(this, option);

    style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &option, &p, this);

    int w = iconSize().width();
    int h = iconSize().height();
    QIcon::Mode mode;
    if ( (option.state & QStyle::State_Enabled) || (option.state & QStyle::State_Active) ) {
        if (option.state & QStyle::State_Sunken)
            mode = QIcon::Selected;
        else if (option.state & QStyle::State_MouseOver)
            mode = QIcon::Active;
        else
            mode = QIcon::Normal;
    } else {
        mode = QIcon::Disabled;
    }

    QRect iconRect((width() - w)/2, (height() - h)/2, w, h);
    icon().paint(&p, iconRect, Qt::AlignCenter, mode);
}
