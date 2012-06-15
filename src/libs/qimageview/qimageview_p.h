#ifndef QIMAGEVIEW_P_H
#define QIMAGEVIEW_P_H

#include "qimageview.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QUndoCommand>

class QListWidget;

// Animations
class ZoomAnimation : public QVariantAnimation
{
    Q_OBJECT
public:
    explicit ZoomAnimation(QImageViewPrivate *dd, QObject *parent = 0);

protected:
    void updateCurrentValue(const QVariant &value);

private:
    QImageViewPrivate *d;
};

class AxisAnimation : public QVariantAnimation
{
    Q_OBJECT

public:
    explicit AxisAnimation(Qt::Axis axis, QImageViewPrivate *dd, QObject *parent = 0);

    Qt::Axis axis() const { return m_axis; }
    qreal angle() const { return currentValue().toReal(); }

protected:
    void updateCurrentValue(const QVariant &value);

private:
    QImageViewPrivate *d;
    Qt::Axis m_axis;
};

// Undo stack commands
class ImageViewCommand : public QUndoCommand
{
public:
    explicit ImageViewCommand(QImageViewPrivate *dd);

protected:
    QImageViewPrivate *d;
};

class RotateCommand : public ImageViewCommand
{
public:
    explicit RotateCommand(bool left, QImageViewPrivate *dd);

    void redo();
    void undo();

private:
    bool m_left;
};

class HFlipCommand : public ImageViewCommand
{
public:
    explicit HFlipCommand(QImageViewPrivate *dd);

    void redo();
    void undo();
};

class VFlipCommand : public ImageViewCommand
{
public:
    explicit VFlipCommand(QImageViewPrivate *dd);

    void redo();
    void undo();
};

class ResetOriginalCommand : public ImageViewCommand
{
public:
    explicit ResetOriginalCommand(const QImage &image, int index, QImageViewPrivate *dd);

    void redo();
    void undo();

private:
    QImage m_image;
    int m_index;
};

class CutCommand : public ImageViewCommand
{
public:
    explicit CutCommand(const QRect &rect, QImageViewPrivate *dd);

    void redo();
    void undo();

private:
    QRect m_rect;
    QImage m_image;
};

class ResizeCommand : public ImageViewCommand
{
public:
    explicit ResizeCommand(const QSize &size, QImageViewPrivate *dd);

    void redo();
    void undo();

private:
    QSize m_size;
    QImage m_image;
};

class QImageViewPrivate
{
    Q_DECLARE_PUBLIC(QImageView)

public:
    explicit QImageViewPrivate(QImageView *qq);

    void recreateViewport(bool useOpenGL);

    void setZoomFactor(qreal factor);
    void setVisualZoomFactor(qreal factor);

    void setCanCopy(bool can);
    void setModified(bool m);
    void setCanWrite(bool can);
    void setCanReset(bool can);

    void rotate(bool left);
    void flipHorizontally();
    void flipVertically();

    void updateScrollBars();
    void updateViewport();
    void animationFinished();
    void undoIndexChanged(int);
    void onMoveToolTriggered(bool);
    void onSelectionToolTriggered(bool);

    void addAxisAnimation(Qt::Axis axis, qreal endValue, int msecs);
    bool hasRunningAnimations() const;
    void stopAnimations();
    void syncPixmap();
    void setImage(const QImage &rimage);
    void updateThumbnailsState();
    void updateThumbnailsGeometry();

    QPointF getCenter() const;
    QRect selectedImageRect() const;
    qreal getFitInViewFactor() const;

    void drawBackground(QPainter *p);
    void drawSelection(QPainter *p);

    void createActions();
    void retranslateUi();
    void updateActions();

public:
    struct ImageData
    {
        ImageData() : nextImageDelay(0) {}
        QImage image;
        QImage originalImage;
        int nextImageDelay;
    };

    QList<ImageData> images;
    int currentImageNumber;
//    QImage image;
    QImage &rimage() { return images[currentImageNumber].image; }
    QImage image() const { return images[currentImageNumber].image; }
    QImageView::MouseMode mouseMode;

    QPixmap pixmap;

    qreal zoomFactor;
    qreal visualZoomFactor;
    ZoomAnimation zoomAnimation;

    QList<AxisAnimation *> runningAnimations;
    int axisAnimationCount;

    bool mousePressed;
    QPoint startPos;
    QPoint pos;
    QPoint prevPos;

    bool canCopy;

    QUndoStack *undoStack;
    int undoStackIndex;
    bool modified;
    bool canWrite;
    bool isZeroModified; // if 0 element in undo stack is an original
    bool canReset;

    QListWidget *listWidget;
    QImageView::Position thumbnailsPosition;

    QAction *actions[QImageView::ActionsCount];

private:
    QImageView *q_ptr;

    friend class ResetOriginalCommand;
};

#endif // QIMAGEVIEW_P_H
