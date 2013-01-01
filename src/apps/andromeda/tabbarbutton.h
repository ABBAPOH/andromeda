#ifndef TABBARBUTTON_H
#define TABBARBUTTON_H

#include <QAbstractButton>

class TabBarButton : public QAbstractButton
{
public:
    explicit TabBarButton(QWidget *parent = 0);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *e);

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    // TODO: use style option to discover state
    bool hovered;
    bool pressed;
};

#endif // TABBARBUTTON_H
