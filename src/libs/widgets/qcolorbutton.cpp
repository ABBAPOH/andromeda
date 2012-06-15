#include "qcolorbutton.h"
#include "qcolorbutton_p.h"

#include <QMimeData>

#include <QApplication>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QPainter>

QColorButtonPrivate::QColorButtonPrivate(QColorButton *qq) :
    q_ptr(qq),
    m_dragging(false),
    dragDropEnabled(false),
    m_backgroundCheckered(true),
    m_alphaAllowed(true)
{
    Q_Q(QColorButton);

    q->setAcceptDrops(true);

    QObject::connect(q, SIGNAL(clicked()), q, SLOT(slotEditColor()));
    q->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
}

void QColorButtonPrivate::slotEditColor()
{
    Q_Q(QColorButton);

    QColor newColor;
    if (m_alphaAllowed) {
        bool ok;
        const QRgb rgba = QColorDialog::getRgba(m_color.rgba(), &ok, q);
        if (!ok)
            return;
        newColor = QColor::fromRgba(rgba);
    } else {
        newColor = QColorDialog::getColor(m_color, q);
        if (!newColor.isValid())
            return;
    }
    if (newColor == q->color())
        return;
    q->setColor(newColor);
    emit q->colorChanged(m_color);
}

QColor QColorButtonPrivate::shownColor() const
{
#ifndef QT_NO_DRAGANDDROP
    if (m_dragging)
        return m_dragColor;
#endif
    return m_color;
}

QPixmap QColorButtonPrivate::generatePixmap() const
{
    QPixmap pix(24, 24);

    int pixSize = 20;
    QBrush br(shownColor());

    QPixmap pm(2 * pixSize, 2 * pixSize);
    QPainter pmp(&pm);
    pmp.fillRect(0, 0, pixSize, pixSize, Qt::lightGray);
    pmp.fillRect(pixSize, pixSize, pixSize, pixSize, Qt::lightGray);
    pmp.fillRect(0, pixSize, pixSize, pixSize, Qt::darkGray);
    pmp.fillRect(pixSize, 0, pixSize, pixSize, Qt::darkGray);
    pmp.fillRect(0, 0, 2 * pixSize, 2 * pixSize, shownColor());
    br = QBrush(pm);

    QPainter p(&pix);
    int corr = 1;
    QRect r = pix.rect().adjusted(corr, corr, -corr, -corr);
    p.setBrushOrigin((r.width() % pixSize + pixSize) / 2 + corr, (r.height() % pixSize + pixSize) / 2 + corr);
    p.fillRect(r, br);

    p.fillRect(r.width() / 4 + corr, r.height() / 4 + corr,
               r.width() / 2, r.height() / 2,
               QColor(shownColor().rgb()));
    p.drawRect(pix.rect().adjusted(0, 0, -1, -1));

    return pix;
}

QColorButton::QColorButton(QWidget *parent) :
    QToolButton(parent),
    d(new QColorButtonPrivate(this))
{
}

QColorButton::QColorButton(const QColor &color, QWidget *parent) :
    QToolButton(parent),
    d(new QColorButtonPrivate(this))
{
    setColor(color);
}

QColorButton::~QColorButton()
{
    delete d;
}

void QColorButton::setColor(const QColor &color)
{
    if (d->m_color == color)
        return;
    d->m_color = color;
    update();
}

QColor QColorButton::color() const
{
    return d->m_color;
}

void QColorButton::setBackgroundCheckered(bool checkered)
{
    if (d->m_backgroundCheckered == checkered)
        return;
    d->m_backgroundCheckered = checkered;
    update();
}

bool QColorButton::isBackgroundCheckered() const
{
    return d->m_backgroundCheckered;
}

void QColorButton::setAlphaAllowed(bool allowed)
{
    d->m_alphaAllowed = allowed;
}

bool QColorButton::isDragDropEnabled() const
{
    return d->dragDropEnabled;
}

void QColorButton::setDragDropEnabled(bool enabled)
{
    d->dragDropEnabled = enabled;
}

bool QColorButton::isAlphaAllowed() const
{
    return d->m_alphaAllowed;
}

void QColorButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);
    if (!isEnabled())
        return;

    const int pixSize = 10;
    QBrush br(d->shownColor());
    if (d->m_backgroundCheckered) {
        QPixmap pm(2 * pixSize, 2 * pixSize);
        QPainter pmp(&pm);
        pmp.fillRect(0, 0, pixSize, pixSize, Qt::white);
        pmp.fillRect(pixSize, pixSize, pixSize, pixSize, Qt::white);
        pmp.fillRect(0, pixSize, pixSize, pixSize, Qt::black);
        pmp.fillRect(pixSize, 0, pixSize, pixSize, Qt::black);
        pmp.fillRect(0, 0, 2 * pixSize, 2 * pixSize, d->shownColor());
        br = QBrush(pm);
    }

    QPainter p(this);
    const int corr = 5;
    QRect r = rect().adjusted(corr, corr, -corr, -corr);
    p.setBrushOrigin((r.width() % pixSize + pixSize) / 2 + corr, (r.height() % pixSize + pixSize) / 2 + corr);
    p.fillRect(r, br);

    //const int adjX = qRound(r.width() / 4.0);
    //const int adjY = qRound(r.height() / 4.0);
    //p.fillRect(r.adjusted(adjX, adjY, -adjX, -adjY),
    //           QColor(d_ptr->shownColor().rgb()));
    /*
    p.fillRect(r.adjusted(0, r.height() * 3 / 4, 0, 0),
               QColor(d_ptr->shownColor().rgb()));
    p.fillRect(r.adjusted(0, 0, 0, -r.height() * 3 / 4),
               QColor(d_ptr->shownColor().rgb()));
               */
    /*
    const QColor frameColor0(0, 0, 0, qRound(0.2 * (0xFF - d_ptr->shownColor().alpha())));
    p.setPen(frameColor0);
    p.drawRect(r.adjusted(adjX, adjY, -adjX - 1, -adjY - 1));
    */

    const QColor frameColor1(0, 0, 0, 26);
    p.setPen(frameColor1);
    p.drawRect(r.adjusted(1, 1, -2, -2));
    const QColor frameColor2(0, 0, 0, 51);
    p.setPen(frameColor2);
    p.drawRect(r.adjusted(0, 0, -1, -1));
}

void QColorButton::mousePressEvent(QMouseEvent *event)
{
#ifndef QT_NO_DRAGANDDROP
    if (event->button() == Qt::LeftButton)
        d->m_dragStart = event->pos();
#endif
    QToolButton::mousePressEvent(event);
}

void QColorButton::mouseMoveEvent(QMouseEvent *event)
{
#ifndef QT_NO_DRAGANDDROP
    if (d->dragDropEnabled &&
            event->buttons() & Qt::LeftButton &&
            (d->m_dragStart - event->pos()).manhattanLength() > QApplication::startDragDistance()) {
        QMimeData *mime = new QMimeData;
        mime->setColorData(color());
        QDrag *drg = new QDrag(this);
        drg->setMimeData(mime);
        drg->setPixmap(d->generatePixmap());
        setDown(false);
        event->accept();
        drg->start();
        return;
    }
#endif
    QToolButton::mouseMoveEvent(event);
}

#ifndef QT_NO_DRAGANDDROP
void QColorButton::dragEnterEvent(QDragEnterEvent *event)
{
    if (!d->dragDropEnabled)
        return;

    const QMimeData *mime = event->mimeData();
    if (!mime->hasColor())
        return;

    event->accept();
    d->m_dragColor = qvariant_cast<QColor>(mime->colorData());
    d->m_dragging = true;
    update();
}

void QColorButton::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    d->m_dragging = false;
    update();
}

void QColorButton::dropEvent(QDropEvent *event)
{
    event->accept();
    d->m_dragging = false;
    if (d->m_dragColor == color())
        return;
    setColor(d->m_dragColor);
    emit colorChanged(color());
}
#endif

#include "qcolorbutton.moc"
