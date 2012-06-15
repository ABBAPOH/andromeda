#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QtGui/QToolButton>

class QColorButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(bool backgroundCheckered READ isBackgroundCheckered WRITE setBackgroundCheckered)
    Q_PROPERTY(bool alphaAllowed READ isAlphaAllowed WRITE setAlphaAllowed)
    Q_PROPERTY(bool dragDropEnabled READ isDragDropEnabled WRITE setDragDropEnabled)

public:
    explicit QColorButton(QWidget *parent = 0);
    explicit QColorButton(const QColor &color, QWidget *parent = 0);
    ~QColorButton();

    bool isBackgroundCheckered() const;
    void setBackgroundCheckered(bool checkered);

    bool isAlphaAllowed() const;
    void setAlphaAllowed(bool allowed);

    bool isDragDropEnabled() const;
    void setDragDropEnabled(bool enabled);

    QColor color() const;

public slots:
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
#endif

private:
    class QColorButtonPrivate *d;
    friend class QColorButtonPrivate;

    Q_DISABLE_COPY(QColorButton)
    Q_PRIVATE_SLOT(d, void slotEditColor())
};

#endif // COLORBUTTON_H
