#ifndef QIMAGEVIEW_H
#define QIMAGEVIEW_H

#include "qimageview_global.h"

#include <QtGui/QAbstractScrollArea>

class QImageViewPrivate;
class QIMAGEVIEW_EXPORT QImageView : public QAbstractScrollArea
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QImageView)

    Q_PROPERTY(MouseMode mouseMode READ mouseMode WRITE setMouseMode NOTIFY mouseModeChanged)
    Q_PROPERTY(bool canCopy READ canRedo NOTIFY canCopyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canWrite READ canWrite NOTIFY canWriteChanged)
    Q_PROPERTY(bool canResetOriginal READ canResetOriginal NOTIFY canResetOriginalChanged)
    Q_PROPERTY(bool modified READ isModified WRITE setModified NOTIFY modifiedChanged)

public:
    enum MouseMode {
        MouseModeMove,
        MouseModeSelect
    };
    Q_ENUMS(MouseMode)

    enum Position {
        North = 0,
        South = 1,
        West = 2,
        East = 3
    };
    Q_ENUMS(Position)

    enum Action {
        Redo,
        Undo,
        Copy,
        Cut,
        MoveTool,
        SelectionTool,

        ZoomIn,
        ZoomOut,
        FitInView,
        NormalSize,

        RotateLeft,
        RotateRight,
        FlipHorizontally,
        FlipVertically,
        ResetOriginal,

        ActionsCount
    };
    Q_ENUMS(Position)

    explicit QImageView(QWidget *parent = 0);
    ~QImageView();

    QAction *action(Action action) const;

    bool canCopy() const;

    bool canRedo() const;
    bool canUndo() const;

    bool canWrite() const;
    bool canResetOriginal() const;

    void read(QIODevice *device, const QByteArray &format = QByteArray());
    void write(QIODevice *device, const QByteArray &format = QByteArray());

    QImage image() const;
    void setImage(const QImage &image);

    int currentImageNumber() const;
    int imageCount() const;

    bool isModified() const;

    MouseMode mouseMode() const;
    void setMouseMode(MouseMode mode);

    QRect selectedImageRect() const;
    QImage selectedImage() const;

    Position thumbnailsPosition() const;
    void setThumbnailsPosition(Position position);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

signals:
    void mouseModeChanged(MouseMode mode);
    void canCopyChanged(bool canCopy);
    void canRedoChanged(bool canRedo);
    void canUndoChanged(bool canUndo);
    void canWriteChanged(bool canWrite);
    void canResetOriginalChanged(bool canReset);
    void modifiedChanged(bool modified);

public slots:
    void setModified(bool modified);

    void zoomIn();
    void zoomOut();
    void bestFit();
    void fitInView();
    void normalSize();

    void jumpToImage(int imageNumber);
    void nextImage();
    void prevImage();

    void resizeImage(const QSize &size);

    void rotateLeft();
    void rotateRight();

    void flipHorizontally();
    void flipVertically();
    void resetOriginal();

    void clearSelection();

    void copy();
    void cut();

    void redo();
    void undo();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void keyPressEvent(QKeyEvent *);

    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    bool viewportEvent(QEvent *);

protected:
    QImageViewPrivate *d_ptr;

    friend class QImageViewSettings;

    Q_PRIVATE_SLOT(d_func(), void updateScrollBars())
    Q_PRIVATE_SLOT(d_func(), void animationFinished())
    Q_PRIVATE_SLOT(d_func(), void undoIndexChanged(int))
    Q_PRIVATE_SLOT(d_func(), void onMoveToolTriggered(bool))
    Q_PRIVATE_SLOT(d_func(), void onSelectionToolTriggered(bool))
};

#endif // QIMAGEVIEW_H
