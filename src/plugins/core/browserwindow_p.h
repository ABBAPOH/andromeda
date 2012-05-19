#ifndef BROWSERWINDOW_P_H
#define BROWSERWINDOW_P_H

#include "browserwindow.h"

#include "addressbar.h"
#include "tabbar.h"

#include <QtCore/QEvent>
#include <QtGui/QToolBar>
#include <guisystem/mainwindowfactory.h>
#include <guisystem/stackedcontainer.h>

#include "tabcontainer.h"

namespace Core {

class BrowserWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(BrowserWindow)
    BrowserWindow *q_ptr;
public:
    BrowserWindowPrivate(BrowserWindow *qq) : q_ptr(qq) {}
    ~BrowserWindowPrivate() {}

    MyAddressBar *lineEdit;
    QToolBar *toolBar;
    TabContainer *container;

    QAction *upAction;
    QAction *newTabAction;

    void setupActions();
    void setupToolBar();
    void setupAlternateToolBar();
    void setupUi();

public slots:
    void onUrlChanged(const QUrl &);

};

class BrowserWindowFactory : public GuiSystem::MainWindowFactory
{
    Q_OBJECT
public:
    explicit BrowserWindowFactory(QObject *parent = 0);

    GuiSystem::MainWindow *create();
    void open(Capability cap, QList<QUrl> urls);
};

} // namespace Core

#endif // BROWSERWINDOW_P_H
