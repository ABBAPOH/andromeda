#ifndef BROWSERWINDOW_P_H
#define BROWSERWINDOW_P_H

#include "browserwindow.h"

#include <QtCore/QEvent>
#include <QtGui/QToolBar>
#include <guisystem/editorwindowfactory.h>
#include <guisystem/stackedcontainer.h>
#include <widgets/tabbar.h>

#include "myaddressbar.h"
#include "tabcontainer.h"

namespace Andromeda {

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
    QAction *prevTabAction;
    QAction *nextTabAction;
    QAction *menuBarAction;

    void setupActions();
    void setupToolBar();
    void setupAlternateToolBar();
    void setupUi();

public slots:
    void onMenuVisibleChanged(bool visible);

};

class BrowserWindowFactory : public GuiSystem::EditorWindowFactory
{
    Q_OBJECT
public:
    explicit BrowserWindowFactory(QObject *parent = 0);

    GuiSystem::EditorWindow *create();

    void open(const QList<QUrl> &urls);
    void openEditor(const QList<QUrl> &urls, const QByteArray &editor);
    void openNewEditor(const QList<QUrl> &urls);
    void openNewWindow(const QList<QUrl> &urls);
};

} // namespace Andromeda

#endif // BROWSERWINDOW_P_H
