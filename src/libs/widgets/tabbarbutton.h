#ifndef TABBARBUTTON_H
#define TABBARBUTTON_H

#include "widgets_global.h"

#include <QAbstractButton>
#include <QTabBar>

class QStyleOption;

class WIDGETS_EXPORT TabBarButton : public QAbstractButton
{
    Q_OBJECT
    Q_DISABLE_COPY(TabBarButton)
public:
    explicit TabBarButton(QWidget *parent = 0);

    QTabBar::Shape shape() const;
    void setShape(QTabBar::Shape shape);

    QSize sizeHint() const;

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QTabBar::Shape m_shape;
};

#endif // TABBARBUTTON_H
