#include "imageview.h"
#include "imageview_p.h"

#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QListWidget>
#include <QtGui/QImageReader>
#include <QtGui/QImageWriter>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollBar>

#include <QtOpenGL/QGLWidget>

#include <GuiSystem/constants.h>

#include "imageviewsettings.h"
#include "imageviewsettings_p.h"

static const qint32 m_magic = 0x71303877; // "q08w"
static const qint8 m_version = 1;

static QPoint adjustPoint(QPoint p, qreal factor)
{
    return QPoint((int)(p.x()/factor)*factor, (int)(p.y()/factor)*factor);
}

static QPixmap chessBoardBackground()
{
    //cbsSize is the size of each square side
    static const int cbsSize = 16;

    QPixmap m = QPixmap(cbsSize*2,cbsSize*2);
    QPainter p(&m);
    p.fillRect(m.rect(), QColor(128,128,128));
    QColor light = QColor(192,192,192);
    p.fillRect(0,0,cbsSize,cbsSize,light);
    p.fillRect(cbsSize,cbsSize,cbsSize,cbsSize, light);
    p.end();
    return m;
}

static QPixmap chessBoardBackground(const QSize &size)
{
    if (size.isEmpty())
        return QPixmap();

    static QSize previousSize;
    static QPixmap cachedPixmap;

    if (size == previousSize) {
        return cachedPixmap;
    }

    int w = size.width(), h = size.height();

    QPixmap background = ::chessBoardBackground();
    QPixmap m(w, h);
    QPainter p(&m);
    p.translate(w/2.0, h/2.0);
    p.drawTiledPixmap(QRect(-8, -8, w/2 + 8, h/2 + 8), background);
    p.rotate(90);
    p.drawTiledPixmap(QRect(-8, -8, w/2 + 8, h/2 + 8), background);
    p.rotate(90);
    p.drawTiledPixmap(QRect(-8, -8, w/2 + 8, h/2 + 8), background);
    p.rotate(90);
    p.drawTiledPixmap(QRect(-8, -8, w/2 + 8, h/2 + 8), background);
    p.end();

    previousSize = size;
    cachedPixmap = m;

    return m;
}

static QPoint containingPoint(QPoint pos, const QRect &rect)
{
    pos.setX(qMax(pos.x(), rect.left()));
    pos.setY(qMax(pos.y(), rect.top()));
    pos.setX(qMin(pos.x(), rect.right()));
    pos.setY(qMin(pos.y(), rect.bottom()));
    return pos;
}

static QDataStream &operator >>(QDataStream & s, ImageViewPrivate::ImageData &data)
{
    s >> data.image;
    s >> data.originalImage;
    s >> data.nextImageDelay;
    return s;
}

static QDataStream &operator <<(QDataStream & s, const ImageViewPrivate::ImageData &data)
{
    s << data.image;
    s << data.originalImage;
    s << data.nextImageDelay;
    return s;
}

ZoomAnimation::ZoomAnimation(ImageViewPrivate *dd, QObject *parent) :
    QVariantAnimation(parent),
    d(dd)
{
}

void ZoomAnimation::updateCurrentValue(const QVariant &value)
{
    d->setVisualZoomFactor(value.toReal());
}

AxisAnimation::AxisAnimation(Qt::Axis axis, ImageViewPrivate *dd, QObject *parent):
    QVariantAnimation(parent),
    d(dd),
    m_axis(axis)
{
}

void AxisAnimation::updateCurrentValue(const QVariant &/*value*/)
{
    d->updateViewport();
}

ImageViewCommand::ImageViewCommand(ImageViewPrivate *dd) :
    QUndoCommand(),
    d(dd)
{
}

RotateCommand::RotateCommand(bool left, ImageViewPrivate *dd) :
    ImageViewCommand(dd),
    m_left(left)
{
}

void RotateCommand::redo()
{
    d->rotate(m_left);
}

void RotateCommand::undo()
{
    d->rotate(!m_left);
}

HFlipCommand::HFlipCommand(ImageViewPrivate *dd) :
    ImageViewCommand(dd)
{
}

void HFlipCommand::redo()
{
    d->flipHorizontally();
}

void HFlipCommand::undo()
{
    d->flipHorizontally();
}

VFlipCommand::VFlipCommand(ImageViewPrivate *dd) :
    ImageViewCommand(dd)
{
}

void VFlipCommand::redo()
{
    d->flipVertically();
}

void VFlipCommand::undo()
{
    d->flipVertically();
}

ResetOriginalCommand::ResetOriginalCommand(const QImage &image, int index, ImageViewPrivate *dd) :
    ImageViewCommand(dd),
    m_image(image),
    m_index(index)
{
}

void ResetOriginalCommand::redo()
{
    d->q_func()->jumpToImage(m_index);
    d->images[m_index].image = d->images[m_index].originalImage;
    d->syncPixmap();
    d->setModified(d->undoStack->index() != d->undoStackIndex);
}

void ResetOriginalCommand::undo()
{
    d->q_func()->jumpToImage(m_index);
    d->images[m_index].image = m_image;
    d->syncPixmap();
    d->setModified(d->undoStack->index() != d->undoStackIndex);
}

CutCommand::CutCommand(const QRect &rect, ImageViewPrivate *dd) :
    ImageViewCommand(dd),
    m_rect(rect)
{
}

void CutCommand::redo()
{
    m_image = d->rimage().copy(m_rect);

    QColor color = QColor(255, 255, 255, d->rimage().hasAlphaChannel() ? 0 : 255);
    for (int x = 0; x < m_rect.width(); x++) {
        for (int y = 0; y < m_rect.height(); y++) {
            d->rimage().setPixel(x + m_rect.x(), y + m_rect.y(), color.rgba());
        }
    }

    d->syncPixmap();
}

void CutCommand::undo()
{
    for (int x = 0; x < m_rect.width(); x++) {
        for (int y = 0; y < m_rect.height(); y++) {
            QRgb color = m_image.pixel(x, y);
            d->rimage().setPixel(m_rect.x() + x, m_rect.y() + y, color);
        }
    }

    d->syncPixmap();
}

ResizeCommand::ResizeCommand(const QSize &size, ImageViewPrivate *dd) :
    ImageViewCommand(dd),
    m_size(size)
{
}

void ResizeCommand::redo()
{
    m_image = d->rimage();
    d->rimage() = d->rimage().scaled(m_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    d->syncPixmap();
}

void ResizeCommand::undo()
{
    d->rimage() = m_image;
    d->syncPixmap();
}

ImageViewPrivate::ImageViewPrivate(ImageView *qq) :
    currentImageNumber(0),
    zoomFactor(1.0),
    visualZoomFactor(1.0),
    zoomAnimation(this),
    axisAnimationCount(0),
    mousePressed(false),
    canCopy(false),
    undoStack(new QUndoStack(qq)),
    undoStackIndex(0),
    modified(0),
    canWrite(false),
    isZeroModified(false),
    canReset(false),
    listWidget(new QListWidget(qq)),
    thumbnailsPosition(ImageView::East),
    q_ptr(qq)
{
    Q_Q(ImageView);

    listWidget->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QPalette palette = listWidget->palette();
    palette.setColor(QPalette::Base, QColor(128, 128, 128));
    listWidget->setPalette(palette);
    listWidget->setGridSize(QSize(100, 100));
    listWidget->setIconSize(QSize(64, 64));
    listWidget->setFlow(QListView::LeftToRight);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setFocusPolicy(Qt::NoFocus);
    QObject::connect(listWidget, SIGNAL(currentRowChanged(int)), q, SLOT(jumpToImage(int)));

    images.append(ImageData());

    QObject::connect(undoStack, SIGNAL(canRedoChanged(bool)), q, SIGNAL(canRedoChanged(bool)));
    QObject::connect(undoStack, SIGNAL(canUndoChanged(bool)), q, SIGNAL(canUndoChanged(bool)));

    QObject::connect(undoStack, SIGNAL(indexChanged(int)), q, SLOT(undoIndexChanged(int)));
}

void ImageViewPrivate::recreateViewport(bool useOpenGL)
{
    Q_Q(ImageView);

    if (useOpenGL) {
        QGLFormat glFormat(QGL::SampleBuffers); // antialiasing
//        glFormat.setSwapInterval(1); // vsync
        q->setViewport(new QGLWidget(glFormat, q));
    } else {
        q->setViewport(new QWidget);
    }
}

void ImageViewPrivate::setZoomFactor(qreal factor)
{
    Q_Q(ImageView);

    if (zoomFactor == factor)
        return;

    if (rimage().isNull())
        return;

    if (factor < 0.01)
        factor = 0.01;

    q->clearSelection();

    zoomFactor = factor;

    if (zoomAnimation.state() == QVariantAnimation::Running)
        zoomAnimation.stop();

    zoomAnimation.setStartValue(visualZoomFactor);
    zoomAnimation.setEndValue(zoomFactor);
    zoomAnimation.setDuration(75);
    zoomAnimation.setEasingCurve(QEasingCurve::Linear);
    zoomAnimation.start();
}

void ImageViewPrivate::setVisualZoomFactor(qreal factor)
{
    visualZoomFactor = factor;

    updateScrollBars();
}

void ImageViewPrivate::setCanCopy(bool can)
{
    Q_Q(ImageView);

    if (canCopy != can) {
        canCopy = can;
        emit q->canCopyChanged(canCopy);
    }
}

void ImageViewPrivate::setModified(bool m)
{
    Q_Q(ImageView);

    if (modified != m) {
        modified = m;
        emit q->modifiedChanged(modified);
    }
}

void ImageViewPrivate::setCanWrite(bool can)
{
    Q_Q(ImageView);

    if (canWrite != can) {
        canWrite = can;
        emit q->canWriteChanged(canWrite);
    }
}

void ImageViewPrivate::setCanReset(bool can)
{
    Q_Q(ImageView);

    if (canReset != can) {
        canReset = can;
        emit q->canResetOriginalChanged(canReset);
    }
}

void ImageViewPrivate::rotate(bool left)
{
    Q_Q(ImageView);

    QTransform matrix;
    matrix.rotate(left ? -90 : 90, Qt::ZAxis);
    rimage() = this->rimage().transformed(matrix, Qt::SmoothTransformation);
    q->viewport()->update();

    addAxisAnimation(Qt::ZAxis, left ? - 90.0 : 90.0, 150);
}

void ImageViewPrivate::flipHorizontally()
{
    QTransform matrix;
    matrix.rotate(180, Qt::YAxis);
    rimage() = rimage().transformed(matrix, Qt::SmoothTransformation);

    addAxisAnimation(Qt::YAxis, 180.0, 200);
}

void ImageViewPrivate::flipVertically()
{
    QTransform matrix;
    matrix.rotate(180, Qt::XAxis);
    rimage() = rimage().transformed(matrix, Qt::SmoothTransformation);

    addAxisAnimation(Qt::XAxis, 180.0, 200);
}

void ImageViewPrivate::updateScrollBars()
{
    Q_Q(ImageView);

    QSize size = pixmap.size() * visualZoomFactor;
    int hmax = size.width() - q->viewport()->width();
    int vmax = size.height() - q->viewport()->height();

    hmax = qMax(0, hmax);
    vmax = qMax(0, vmax);

    q->horizontalScrollBar()->setRange(-hmax/2.0, hmax/2.0 + 0.5);
    q->verticalScrollBar()->setRange(-vmax/2.0, vmax/2.0 + 0.5);

    q->viewport()->update();
}

void ImageViewPrivate::updateViewport()
{
    Q_Q(ImageView);
    q->viewport()->update();
}

void ImageViewPrivate::animationFinished()
{
    axisAnimationCount--;
    if (!axisAnimationCount)
        syncPixmap();
}

void ImageViewPrivate::undoIndexChanged(int index)
{
    setModified(index != undoStackIndex);
    setCanReset(true);
}

void ImageViewPrivate::onMoveToolTriggered(bool triggered)
{
    Q_Q(ImageView);

    if (triggered)
        q->setMouseMode(ImageView::MouseModeMove);
}

void ImageViewPrivate::onSelectionToolTriggered(bool triggered)
{
    Q_Q(ImageView);

    if (triggered)
        q->setMouseMode(ImageView::MouseModeSelect);
}

void ImageViewPrivate::addAxisAnimation(Qt::Axis axis, qreal endValue, int msecs)
{
    Q_Q(ImageView);

    AxisAnimation *animation = new AxisAnimation(axis, this, q);
    animation->setStartValue(0.0);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(msecs);
    animation->start();
    runningAnimations.append(animation);
    axisAnimationCount++;
    QObject::connect(animation, SIGNAL(finished()), q, SLOT(animationFinished()));
}

bool ImageViewPrivate::hasRunningAnimations() const
{
    return axisAnimationCount || (zoomAnimation.state() == QVariantAnimation::Running);
}

void ImageViewPrivate::stopAnimations()
{
    foreach (AxisAnimation *animation, runningAnimations) {
        animation->stop();
    }

    qDeleteAll(runningAnimations);
    runningAnimations.clear();
    axisAnimationCount = 0;
}

void ImageViewPrivate::syncPixmap()
{
    pixmap = QPixmap::fromImage(rimage());

    stopAnimations();
    updateViewport();
}

void ImageViewPrivate::setImage(const QImage &image)
{
    this->rimage() = image;

    syncPixmap();
}

void ImageViewPrivate::updateThumbnailsState()
{
    Q_Q(ImageView);

    switch (thumbnailsPosition) {
    case ImageView::North:
    case ImageView::South:
        listWidget->setFlow(QListView::LeftToRight);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        break;
    case ImageView::West:
    case ImageView::East:
        listWidget->setFlow(QListView::TopToBottom);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        break;
    default:
        break;
    }

    if (q->imageCount() > 1) {
        QMargins margins(0, 0, 0, 0);
        switch (thumbnailsPosition) {
        case ImageView::North: margins.setTop(100); break;
        case ImageView::South: margins.setBottom(100); break;
        case ImageView::West: margins.setLeft(100); break;
        case ImageView::East: margins.setRight(100); break;
        default: break;
        }
        q->setViewportMargins(margins);
        listWidget->setVisible(true);
    } else {
        q->setViewportMargins(0, 0, 0, 0);
        listWidget->setVisible(false);
    }
}

void ImageViewPrivate::updateThumbnailsGeometry()
{
    Q_Q(ImageView);

    QRect rect = q->rect();
    switch (thumbnailsPosition) {
    case ImageView::North: rect.setHeight(100); break;
    case ImageView::South: rect.setY(rect.y() + rect.height() - 100); break;
    case ImageView::West: rect.setWidth(100); break;
    case ImageView::East: rect.setX(rect.x() + rect.width() - 100); break;
    default: break;
    }
    listWidget->setGeometry(rect);
}

QPointF ImageViewPrivate::getCenter() const
{
    Q_Q(const ImageView);

    int hvalue = q->horizontalScrollBar()->value();
    int vvalue = q->verticalScrollBar()->value();

    QSizeF size = q->viewport()->size()/2.0 - QSizeF(hvalue, vvalue);

    return QPointF(size.width(), size.height());
}

QRect ImageViewPrivate::selectedImageRect() const
{
    if (startPos == pos)
        return QRect();

    QPointF center = getCenter();

    QRectF selectionRect(startPos, pos);
    selectionRect = selectionRect.normalized();
    selectionRect.translate(-center.x(), -center.y());

    qreal factor = visualZoomFactor;
    selectionRect = QRectF(selectionRect.topLeft()/factor, selectionRect.bottomRight()/factor);

    QRectF pixmapRect(QPointF(0, 0), pixmap.size());
    pixmapRect.translate(-pixmapRect.center());

    QRectF result = QRectF(selectionRect.intersect(pixmapRect));
    result.translate(pixmap.width()/2.0, pixmap.height()/2.0);
    return result.toRect();
}

qreal ImageViewPrivate::getFitInViewFactor() const
{
    Q_Q(const ImageView);

    QSize imageSize = image().size();
    if (imageSize.isEmpty())
        return 1.0;

    QSize size = q->maximumViewportSize();

    int w = imageSize.width(), mw = size.width();
    int h = imageSize.height(), mh = size.height();

    double rw = 1.0*mw/w, rh = 1.0*mh/h, factor = 1;

    if (rw < 1 && rh > 1)
        factor = rw;
    else if (rw > 1 && rh < 1)
        factor = rh;
    else if (rw < 1 && rh < 1)
        factor = qMin(rw, rh);
    else
        factor = qMin(rw, rh);

    return factor;
}

void ImageViewPrivate::drawBackground(QPainter *p)
{
    ImageViewSettings *settings = ImageViewSettings::globalSettings();

    ImageViewSettings::ImageBackgroundType type = settings->imageBackgroundType();
    QColor imageBackgroundColor = settings->imageBackgroundColor();

    QSize size = pixmap.size()*visualZoomFactor;
    QRectF rect(QPointF(0, 0), size);
    rect.translate(-rect.center());

    switch (type) {
    case ImageViewSettings::None :
        break;
    case ImageViewSettings::Chess :
        p->drawPixmap(rect, chessBoardBackground(size), QRectF(QPointF(0, 0), size));
        break;
    case ImageViewSettings::SolidColor :
        p->fillRect(rect, imageBackgroundColor);
        break;
    }
}

void ImageViewPrivate::drawSelection(QPainter *p)
{
    Q_Q(ImageView);

    if (mouseMode != ImageView::MouseModeSelect)
        return;

    if (startPos == pos)
        return;

    QPointF center = getCenter();

    QRect rect = q->viewport()->rect();
    rect.translate(-center.x(), -center.y());

    qreal factor = visualZoomFactor;
    QSize backgroundSize = pixmap.size()*factor;
    QRect imageRect(QPoint(0, 0), backgroundSize);
    imageRect.translate(-imageRect.center());

    // rect in painter's coordinates
    QRect selectionRect(::adjustPoint(startPos, zoomFactor), ::adjustPoint(pos, zoomFactor));
    selectionRect.translate(-center.x(), -center.y());
    selectionRect = selectionRect.intersect(rect);

    if (selectionRect.isNull())
        return;

    p->setPen(QPen(Qt::lightGray, 1, Qt::DashLine, Qt::RoundCap));
    p->drawRect(selectionRect);

    QRect imageSelectionRect = imageRect.intersect(selectionRect);

    p->setPen(QPen(Qt::black, 1, Qt::DashLine, Qt::RoundCap));
    p->drawRect(imageSelectionRect);

    QString text = ImageView::tr("%1 x %2").
            arg(abs(imageSelectionRect.width()/visualZoomFactor)).
            arg(abs(imageSelectionRect.height()/visualZoomFactor));

    int textWidth = p->fontMetrics().width(text);
    int textHeight = p->fontMetrics().height();

    QPoint textPos = pos + rect.topLeft();
    textPos = containingPoint(textPos, rect);

    textPos.setX(qMax(textPos.x(), rect.left()));
    textPos.setY(qMax(textPos.y(), rect.top() + textHeight));
    textPos.setX(qMin(textPos.x(), rect.right() - textWidth));
    textPos.setY(qMin(textPos.y(), rect.bottom() - textHeight));

    p->setPen(Qt::black);
    p->drawText(textPos, text);
}

void ImageViewPrivate::createActions()
{
    Q_Q(ImageView);

    actions[ImageView::Redo] = new QAction(q);
    actions[ImageView::Redo]->setObjectName(Constants::Actions::Redo);
    actions[ImageView::Redo]->setShortcut(QKeySequence::Redo);
    q->connect(actions[ImageView::Redo], SIGNAL(triggered()), q, SLOT(redo()));
    q->connect(q, SIGNAL(canRedoChanged(bool)), actions[ImageView::Redo], SLOT(setEnabled(bool)));

    actions[ImageView::Undo] = new QAction(q);
    actions[ImageView::Undo]->setObjectName(Constants::Actions::Undo);
    actions[ImageView::Undo]->setShortcut(QKeySequence::Undo);
    q->connect(actions[ImageView::Undo], SIGNAL(triggered()), q, SLOT(undo()));
    q->connect(q, SIGNAL(canUndoChanged(bool)), actions[ImageView::Undo], SLOT(setEnabled(bool)));

    actions[ImageView::Copy] = new QAction(q);
    actions[ImageView::Copy]->setObjectName(Constants::Actions::Copy);
    actions[ImageView::Copy]->setShortcut(QKeySequence::Copy);
    actions[ImageView::Copy]->setEnabled(false);
    q->connect(actions[ImageView::Copy], SIGNAL(triggered()), q, SLOT(copy()));
    q->connect(q, SIGNAL(canCopyChanged(bool)), actions[ImageView::Copy], SLOT(setEnabled(bool)));

    actions[ImageView::Cut] = new QAction(q);
    actions[ImageView::Cut]->setObjectName(Constants::Actions::Cut);
    actions[ImageView::Cut]->setShortcut(QKeySequence::Cut);
    actions[ImageView::Cut]->setEnabled(false);
    q->connect(actions[ImageView::Cut], SIGNAL(triggered()), q, SLOT(cut()));
    q->connect(q, SIGNAL(canCopyChanged(bool)), actions[ImageView::Cut], SLOT(setEnabled(bool)));

    actions[ImageView::MoveTool] = new QAction(q);
    actions[ImageView::MoveTool]->setObjectName("MoveTool");
    actions[ImageView::MoveTool]->setCheckable(true);
    actions[ImageView::MoveTool]->setChecked(true);
    actions[ImageView::MoveTool]->setShortcut(QKeySequence("Ctrl+1"));
    q->connect(actions[ImageView::MoveTool], SIGNAL(triggered(bool)), q, SLOT(onMoveToolTriggered(bool)));

    actions[ImageView::SelectionTool] = new QAction(q);
    actions[ImageView::SelectionTool]->setObjectName("SelectionTool");
    actions[ImageView::SelectionTool]->setCheckable(true);
    actions[ImageView::SelectionTool]->setShortcut(QKeySequence("Ctrl+2"));
    q->connect(actions[ImageView::SelectionTool], SIGNAL(triggered(bool)), q, SLOT(onSelectionToolTriggered(bool)));

    QActionGroup *toolGroup = new QActionGroup(q);
    toolGroup->setExclusive(true);
    toolGroup->addAction(actions[ImageView::MoveTool]);
    toolGroup->addAction(actions[ImageView::SelectionTool]);

    actions[ImageView::ZoomIn] = new QAction(q);
    actions[ImageView::ZoomIn]->setObjectName(Constants::Actions::ZoomIn);
    actions[ImageView::ZoomIn]->setShortcut(QKeySequence::ZoomIn);
    q->connect(actions[ImageView::ZoomIn], SIGNAL(triggered()), q, SLOT(zoomIn()));

    actions[ImageView::ZoomOut] = new QAction(q);
    actions[ImageView::ZoomOut]->setObjectName(Constants::Actions::ZoomOut);
    actions[ImageView::ZoomOut]->setShortcut(QKeySequence::ZoomOut);
    q->connect(actions[ImageView::ZoomOut], SIGNAL(triggered()), q, SLOT(zoomOut()));

    actions[ImageView::FitInView] = new QAction(q);
    actions[ImageView::FitInView]->setObjectName(Constants::Actions::FitInView);
    actions[ImageView::FitInView]->setShortcut(QKeySequence("Ctrl+9"));
    q->connect(actions[ImageView::FitInView], SIGNAL(triggered()), q, SLOT(fitInView()));

    actions[ImageView::NormalSize] = new QAction(q);
    actions[ImageView::NormalSize]->setObjectName(Constants::Actions::NormalSize);
    actions[ImageView::NormalSize]->setShortcut(QKeySequence("Ctrl+0"));
    q->connect(actions[ImageView::NormalSize], SIGNAL(triggered()), q, SLOT(normalSize()));

    actions[ImageView::RotateLeft] = new QAction(q);
    actions[ImageView::RotateLeft]->setObjectName("RotateLeft");
    actions[ImageView::RotateLeft]->setShortcut(QKeySequence("Ctrl+L"));
    q->connect(actions[ImageView::RotateLeft], SIGNAL(triggered()), q, SLOT(rotateLeft()));

    actions[ImageView::RotateRight] = new QAction(q);
    actions[ImageView::RotateRight]->setObjectName("RotateRight");
    actions[ImageView::RotateRight]->setShortcut(QKeySequence("Ctrl+R"));
    q->connect(actions[ImageView::RotateRight], SIGNAL(triggered()), q, SLOT(rotateRight()));

    actions[ImageView::FlipHorizontally] = new QAction(q);
    actions[ImageView::FlipHorizontally]->setObjectName("FlipHorizontally");
    actions[ImageView::FlipHorizontally]->setShortcut(QKeySequence("Ctrl+Shift+H"));
    q->connect(actions[ImageView::FlipHorizontally], SIGNAL(triggered()), q, SLOT(flipHorizontally()));

    actions[ImageView::FlipVertically] = new QAction(q);
    actions[ImageView::FlipVertically]->setObjectName("FlipVertically");
    actions[ImageView::FlipVertically]->setShortcut(QKeySequence("Ctrl+Shift+V"));
    q->connect(actions[ImageView::FlipVertically], SIGNAL(triggered()), q, SLOT(flipVertically()));

    actions[ImageView::ResetOriginal] = new QAction(q);
    actions[ImageView::ResetOriginal]->setObjectName("ResetOriginal");
    actions[ImageView::ResetOriginal]->setEnabled(false);
    q->connect(actions[ImageView::ResetOriginal], SIGNAL(triggered()), q, SLOT(resetOriginal()));
    q->connect(q, SIGNAL(canResetOriginalChanged(bool)),
               actions[ImageView::ResetOriginal], SLOT(setEnabled(bool)));

    for (int i = 0; i < ImageView::ActionsCount; ++i) {
        actions[i]->setShortcutContext(Qt::WidgetShortcut);
        q->addAction(actions[i]);
    }

    updateActions();
}

void ImageViewPrivate::retranslateUi()
{
    actions[ImageView::Redo]->setText(ImageView::tr("Redo"));
    actions[ImageView::Undo]->setText(ImageView::tr("Undo"));
    actions[ImageView::Copy]->setText(ImageView::tr("Copy"));
    actions[ImageView::Cut]->setText(ImageView::tr("Cut"));
    actions[ImageView::MoveTool]->setText(ImageView::tr("Move tool"));
    actions[ImageView::SelectionTool]->setText(ImageView::tr("Selection tool"));

    actions[ImageView::ZoomIn]->setText(ImageView::tr("Zoom in"));
    actions[ImageView::ZoomOut]->setText(ImageView::tr("Zoom out"));
    actions[ImageView::FitInView]->setText(ImageView::tr("Fit in view"));
    actions[ImageView::NormalSize]->setText(ImageView::tr("Normal size"));

    actions[ImageView::RotateLeft]->setText(ImageView::tr("Rotate left"));
    actions[ImageView::RotateRight]->setText(ImageView::tr("Rotate right"));
    actions[ImageView::FlipHorizontally]->setText(ImageView::tr("Flip horizontally"));
    actions[ImageView::FlipVertically]->setText(ImageView::tr("Flip vertically"));
    actions[ImageView::ResetOriginal]->setText(ImageView::tr("Reset original"));
}

void ImageViewPrivate::updateActions()
{
    bool enabled = !image().isNull();

    if (!enabled) {
        actions[ImageView::Redo]->setEnabled(false);
        actions[ImageView::Undo]->setEnabled(false);
    }

    actions[ImageView::ZoomIn]->setEnabled(enabled);
    actions[ImageView::ZoomOut]->setEnabled(enabled);
    actions[ImageView::FitInView]->setEnabled(enabled);
    actions[ImageView::NormalSize]->setEnabled(enabled);

    actions[ImageView::RotateLeft]->setEnabled(enabled);
    actions[ImageView::RotateRight]->setEnabled(enabled);
    actions[ImageView::FlipHorizontally]->setEnabled(enabled);
    actions[ImageView::FlipVertically]->setEnabled(enabled);
}

ImageView::ImageView(QWidget *parent) :
    QAbstractScrollArea(parent),
    d_ptr(new ImageViewPrivate(this))
{
    Q_D(ImageView);

    horizontalScrollBar()->setSingleStep(10);
    verticalScrollBar()->setSingleStep(10);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setMouseMode(MouseModeMove);

    ImageViewSettings *settings = ImageViewSettings::globalSettings();
    if (settings->useOpenGL())
        d->recreateViewport(true);
    settings->d_func()->addView(this);

    d->updateThumbnailsState();

    d->createActions();
    d->retranslateUi();
}

ImageView::~ImageView()
{
    ImageViewSettings::globalSettings()->d_func()->removeView(this);
    delete d_ptr;
}

QAction * ImageView::action(ImageView::Action action) const
{
    Q_D(const ImageView);

    if (action < 0 || action >= ActionsCount)
        return 0;

    return d->actions[action];
}

bool ImageView::canCopy() const
{
    Q_D(const ImageView);

    return d->canCopy;
}

bool ImageView::canRedo() const
{
    Q_D(const ImageView);

    return d->undoStack->canRedo();
}

bool ImageView::canUndo() const
{
    Q_D(const ImageView);

    return d->undoStack->canUndo();
}

bool ImageView::canWrite() const
{
    Q_D(const ImageView);

    return d->canWrite;
}

bool ImageView::canResetOriginal() const
{
    Q_D(const ImageView);

    return d->canReset;
}

void ImageView::read(QIODevice *device, const QByteArray &format)
{
    Q_D(ImageView);

    d->images.clear();
    d->listWidget->clear();
    d->undoStack->clear();
    d->isZeroModified = false;
    d->setCanReset(false);

    QImageReader reader(device, format);
    for (int i = 0; i < reader.imageCount(); ++i) {
        ImageViewPrivate::ImageData data;
        data.image = reader.read();
        data.originalImage = data.image;
        data.nextImageDelay = reader.nextImageDelay();
        d->images.append(data);

        QListWidgetItem *item = new QListWidgetItem(d->listWidget);
        item->setIcon(QIcon(QPixmap::fromImage(data.image)));
        d->listWidget->addItem(item);
    }

    if (d->images.isEmpty()) {
        d->images.append(ImageViewPrivate::ImageData());
        d->zoomFactor = 1.0;
        d->visualZoomFactor = 1.0;
        d->updateScrollBars();
        d->updateActions();
        d->setCanWrite(false);
        return;
    }

    d->setImage(d->images.first().image);

    d->setCanWrite(imageCount() == 1);
    d->updateThumbnailsState();
    bestFit();
    viewport()->update();
    d->updateActions();
}

void ImageView::write(QIODevice *device, const QByteArray &format)
{
    Q_D(ImageView);

    QImageWriter writer(device, format);
    writer.write(d->rimage());
}

QImage ImageView::image() const
{
    Q_D(const ImageView);

    return d->image();
}

void ImageView::setImage(const QImage &image)
{
    Q_D(ImageView);

    d->images.clear();
    d->undoStack->clear();
    d->listWidget->clear();
    d->isZeroModified = false;
    d->setCanReset(false);

    if (image.isNull()) {
        d->images.append(ImageViewPrivate::ImageData());
        d->setImage(QImage());
        d->currentImageNumber = 0;
        d->zoomFactor = 1.0;
        d->visualZoomFactor = 1.0;
        d->updateScrollBars();
        d->setCanWrite(false);
        d->updateActions();
        return;
    }

    d->setImage(image);
    ImageViewPrivate::ImageData data;
    data.image = image;
    data.originalImage = image;
    data.nextImageDelay = 0;
    d->images.append(data);
    d->currentImageNumber = 0;
    d->setCanWrite(true);

    d->updateThumbnailsState();
    bestFit();
    viewport()->update();
    d->updateActions();
}

int ImageView::currentImageNumber() const
{
    Q_D(const ImageView);

    return d->currentImageNumber;
}

int ImageView::imageCount() const
{
    Q_D(const ImageView);

    return d->images.count();
}

bool ImageView::isModified() const
{
    Q_D(const ImageView);

    return d->modified;
}

void ImageView::setModified(bool modified)
{
    Q_D(ImageView);

    if (modified)
        d->undoStackIndex = -1; // we always have modified file
    else
        d->undoStackIndex = d->undoStack->index();

    d->setModified(modified);
}

ImageView::MouseMode ImageView::mouseMode() const
{
    Q_D(const ImageView);

    return d->mouseMode;
}

void ImageView::setMouseMode(ImageView::MouseMode mode)
{
    Q_D(ImageView);

    if (d->mouseMode != mode) {
        if (mode == MouseModeMove)
            viewport()->setCursor(Qt::OpenHandCursor);
        else
            viewport()->setCursor(Qt::ArrowCursor);

        clearSelection();

        d->mouseMode = mode;
        emit mouseModeChanged(mode);
    }
}

QRect ImageView::selectedImageRect() const
{
    Q_D(const ImageView);

    return d->selectedImageRect();
}

QImage ImageView::selectedImage() const
{
    Q_D(const ImageView);

    return d->image().copy(selectedImageRect());
}

ImageView::Position ImageView::thumbnailsPosition() const
{
    Q_D(const ImageView);

    return d->thumbnailsPosition;
}

void ImageView::setThumbnailsPosition(ImageView::Position position)
{
    Q_D(ImageView);

    if (d->thumbnailsPosition == position)
        return;

    d->thumbnailsPosition = position;
    d->updateThumbnailsState();
}

QByteArray ImageView::saveState() const
{
    Q_D(const ImageView);

    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);

    s << m_magic;
    s << m_version;
    s << d->images;
    s << (qint32)d->currentImageNumber;
    s << d->zoomFactor;
    s << d->canReset;

    return result;
}

bool ImageView::restoreState(const QByteArray &arr)
{
    Q_D(ImageView);

    QByteArray state(arr);
    QDataStream s(&state, QIODevice::ReadOnly);

    qint32 imageNumber, magic;
    qint8 version;
    bool canReset;

    s >> magic;
    if (magic != m_magic)
        return false;

    s >> version;
    if (version != m_version)
        return false;

    QList<ImageViewPrivate::ImageData> images;
    s >> images;
    if (images.isEmpty())
        return false;
    qSwap(d->images, images);

    s >> imageNumber;
    s >> d->zoomFactor;
    s >> canReset;
    d->visualZoomFactor = d->zoomFactor;

    d->currentImageNumber = -1;
    jumpToImage(imageNumber);

    d->setCanWrite(imageCount() == 1);
    d->setCanReset(canReset);
    d->updateThumbnailsState();
    d->updateActions();

    return true;
}

void ImageView::zoomIn()
{
    Q_D(ImageView);

    d->setZoomFactor(d->zoomFactor*1.2);
}

void ImageView::zoomOut()
{
    Q_D(ImageView);

    d->setZoomFactor(d->zoomFactor*0.8);
}

void ImageView::bestFit()
{
    Q_D(ImageView);

    if (d->rimage().isNull())
        return;

    qreal factor = d->getFitInViewFactor();
    factor = qMin(factor, 1.0);
    d->setZoomFactor(factor);
}

void ImageView::fitInView()
{
    Q_D(ImageView);

    if (d->rimage().isNull())
        return;

    qreal factor = d->getFitInViewFactor();
    d->setZoomFactor(factor);
}

void ImageView::normalSize()
{
    Q_D(ImageView);

    d->setZoomFactor(1.0);
}

void ImageView::jumpToImage(int imageNumber)
{
    Q_D(ImageView);

    if (d->currentImageNumber == imageNumber)
        return;

    if (imageNumber < 0 || imageNumber >= imageCount())
        return;

    d->currentImageNumber = imageNumber;
    d->listWidget->setCurrentIndex(d->listWidget->model()->index(imageNumber, 0, QModelIndex()));
    d->setImage(d->images.at(imageNumber).image);
}

void ImageView::nextImage()
{
    int count = imageCount();
    if (!count)
        return;

    jumpToImage((currentImageNumber() + 1) % count);
}

void ImageView::prevImage()
{
    int count = imageCount();
    if (!count)
        return;

    jumpToImage((currentImageNumber() - 1 + count) % count);
}

void ImageView::resizeImage(const QSize &size)
{
    Q_D(ImageView);

    if (size.isEmpty())
        return;

    d->undoStack->push(new ResizeCommand(size, d));
}

void ImageView::rotateLeft()
{
    Q_D(ImageView);

    d->undoStack->push(new RotateCommand(true, d));
}

void ImageView::rotateRight()
{
    Q_D(ImageView);

    d->undoStack->push(new RotateCommand(false, d));
}

void ImageView::flipHorizontally()
{
    Q_D(ImageView);

    d->undoStack->push(new HFlipCommand(d));
}

void ImageView::flipVertically()
{
    Q_D(ImageView);

    d->undoStack->push(new VFlipCommand(d));
}

void ImageView::resetOriginal()
{
    Q_D(ImageView);

    d->undoStack->push(new ResetOriginalCommand(d->image(), d->currentImageNumber, d));
}

void ImageView::clearSelection()
{
    Q_D(ImageView);

    d->startPos = d->pos = QPoint();
    d->setCanCopy(false);
    viewport()->update();
}

void ImageView::copy()
{
    QImage image = selectedImage();

    QClipboard *clipboard = qApp->clipboard();
    clipboard->clear();
    clipboard->setImage(image);
}

void ImageView::cut()
{
    Q_D(ImageView);

    copy();

    d->undoStack->push(new CutCommand(selectedImageRect(), d));
}

void ImageView::redo()
{
    Q_D(ImageView);

    d->undoStack->redo();
}

void ImageView::undo()
{
    Q_D(ImageView);

    d->undoStack->undo();
}

void ImageView::mousePressEvent(QMouseEvent *e)
{
    Q_D(ImageView);

    d->mousePressed = true;
    d->startPos = e->pos();
    d->pos = e->pos();
    d->prevPos = e->pos();

    if (d->mouseMode == MouseModeMove)
        viewport()->setCursor(Qt::ClosedHandCursor);
    else
        d->setCanCopy(false);

    viewport()->update();
}

void ImageView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(ImageView);

    QPoint pos = e->pos();

    int dx = d->prevPos.x() - pos.x();
    int dy = d->prevPos.y() - pos.y();

    d->pos = pos;
    d->prevPos = pos;

    if (d->mouseMode == MouseModeMove) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dx);
        verticalScrollBar()->setValue(verticalScrollBar()->value() + dy);
    } else {
        d->setCanCopy(d->pos != d->startPos);
    }

    viewport()->update();
}

void ImageView::mouseReleaseEvent(QMouseEvent *)
{
    Q_D(ImageView);

    d->prevPos = QPoint();

    if (d->mouseMode == MouseModeMove)
        viewport()->setCursor(Qt::OpenHandCursor);
    d->mousePressed = false;

    viewport()->update();
}

void ImageView::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Escape :
        clearSelection();
        break;
    default:
        break;
    }

    QAbstractScrollArea::keyPressEvent(e);
}

void ImageView::paintEvent(QPaintEvent *)
{
    Q_D(ImageView);

    QPainter p(viewport());
    if (!d->hasRunningAnimations())
        p.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    QRect rect = viewport()->rect();

    // Draw viewport background
    QColor backgroundColor = ImageViewSettings::globalSettings()->backgroundColor();
    p.fillRect(rect, backgroundColor);

    if (d->image().isNull())
        return;

    // Move and rotate painter
    QPointF center = d->getCenter();

    QTransform matrix;
    matrix.translate(center.x(), center.y());

    for (int i = d->runningAnimations.count() - 1; i >= 0; i--) {
        AxisAnimation *animation = d->runningAnimations.at(i);
        matrix.rotate(animation->angle(), animation->axis());
    }

    p.setTransform(matrix);

    // Draw image background
    d->drawBackground(&p);

    // Draw scaled pixmap
    p.save();
    p.scale(d->visualZoomFactor, d->visualZoomFactor);

    QRectF pixmapRect(QPointF(0, 0), d->pixmap.size());
    pixmapRect.translate(-pixmapRect.center());
    p.drawPixmap(pixmapRect, d->pixmap, QRectF(QPointF(0, 0), d->pixmap.size()));

    p.restore();

    // Draw vieport's and pixmap's selections
    d->drawSelection(&p);
}

void ImageView::resizeEvent(QResizeEvent *)
{
    Q_D(ImageView);

    d->updateThumbnailsGeometry();
}

bool ImageView::viewportEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::Resize : {
        Q_D(ImageView);

        d->updateScrollBars();
    }
    default:
        break;
    }

    return QAbstractScrollArea::viewportEvent(e);
}

#include "imageview.moc"
