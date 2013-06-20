#ifndef BROWSERWINDOW_P_H
#define BROWSERWINDOW_P_H

#include "browserwindow.h"

#include <QtCore/QEvent>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QToolBar>
#else
#include <QtGui/QToolBar>
#endif

#include <Parts/EditorWindowFactory>
#include <Parts/EditorView>
#include <Parts/HistoryButton>
#include <Widgets/TabBar>

#include "browsertabwidget.h"
#include "myaddressbar.h"

namespace Andromeda {

class BrowserWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(BrowserWindow)
    BrowserWindow *q_ptr;
public:
    BrowserWindowPrivate(BrowserWindow *qq) : q_ptr(qq) {}
    ~BrowserWindowPrivate() {}

    QToolBar *toolBar;
    BrowserTabWidget *tabWidget;
    Parts::History *history;

    QAction *actions[BrowserWindow::ActionCount];

    void setupActions();
    void setupToolBar();
    void setupAlternateToolBar();
    void setupUi();
    void retranslateUi();

public slots:
    void onMenuVisibleChanged(bool visible);

};

class BrowserWindowFactory : public Parts::EditorWindowFactory
{
    Q_OBJECT
public:
    explicit BrowserWindowFactory(QObject *parent = 0);

    Parts::EditorWindow *create();

    void open(const QList<QUrl> &urls);
    void openEditor(const QList<QUrl> &urls, const QByteArray &editor);
    void openNewEditor(const QList<QUrl> &urls);
    void openNewWindow(const QList<QUrl> &urls);
};

} // namespace Andromeda

#endif // BROWSERWINDOW_P_H
