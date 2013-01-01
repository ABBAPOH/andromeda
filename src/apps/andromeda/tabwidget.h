#ifndef TABCONTAINER_P_H
#define TABCONTAINER_P_H

#include <QtGui/QTabWidget>
#include <QtGui/QAbstractButton>

#include "tabbar.h"

class MyTabWidget : public QTabWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MyTabWidget)

    Q_PROPERTY(bool tabBarVisible READ isTabBarVisible WRITE setTabBarVisible NOTIFY tabBarVisibleChanged)
public:
    explicit MyTabWidget(QWidget *parent = 0);

    bool isTabBarVisible() const;
    void setTabBarVisible(bool visible);

signals:
    void tabBarDoubleClicked();
    void tabBarVisibleChanged(bool visible);

protected:
    bool eventFilter(QObject *o, QEvent *e);
};

#endif // TABCONTAINER_P_H
