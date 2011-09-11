#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "mainwindow.h"

#include "tabbar.h"

#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <enteredlineedit.h>
#include <CorePlugin>

class MyTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    MyTabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        TabBar *tabBar = new TabBar;
        tabBar->setSwitchTabsOnDrag(true);
        tabBar->installEventFilter(this);
        setTabBar(tabBar);
    }

signals:
    void tabBarDoubleClicked();

protected:
    bool eventFilter(QObject *o, QEvent *e)
    {
        if (o != tabBar())
            return false;

        if (e->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *me = static_cast<QMouseEvent *>(e);

            if (tabBar()->tabAt(me->pos()) == -1)
                emit tabBarDoubleClicked();

            return true;
        }
        return false;
    }
};

namespace CorePlugin {

class MainWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(MainWindow)
    MainWindow *q_ptr;
public:
    MainWindowPrivate(MainWindow *qq) : q_ptr(qq) {}

    EnteredLineEdit *lineEdit;
    QToolBar *toolBar;
    QTabWidget *tabWidget;

    QAction *backAction;
    QAction *forwardAction;
    QAction *upAction;

    QAction *nextTabAction;
    QAction *prevTabAction;

    Tab *addTab(int *index = 0);
    void setupActions();
    void setupToolBar();
    void setupUi();

public slots:
    void onPathChanged(const QString &);
    void onCurrentChanged(int);
    void onChanged();
};

} // namespace CorePlugin

#endif // MAINWINDOW_P_H
