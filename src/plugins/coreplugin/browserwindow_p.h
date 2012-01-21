#ifndef BROWSERWINDOW_P_H
#define BROWSERWINDOW_P_H

#include "browserwindow.h"

#include "tabbar.h"

#include <QtCore/QEvent>
#include <QtGui/QToolBar>
#include <widgets/addressbar.h>
#include <guisystem/stackedcontainer.h>

#include "tabwidget.h"

namespace CorePlugin {

class BrowserWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(BrowserWindow)
    BrowserWindow *q_ptr;
public:
    BrowserWindowPrivate(BrowserWindow *qq) : q_ptr(qq) {}

    AddressBar *lineEdit;
    QToolBar *toolBar;
    QTabWidget *tabWidget;
    TabBarButton *newTabButton;

    QAction *backAction;
    QAction *forwardAction;
    QAction *upAction;

    QAction *nextTabAction;
    QAction *prevTabAction;

    QAction *newTabAction;
    QAction *closeTabAction;

    QAction *saveAction;
    QAction *saveAsAction;

    QAction *undoAction;
    QAction *redoAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;

    QSettings *settings;

    GuiSystem::StackedContainer *addTab(int *index = 0);

    void createAction(QAction *&action, const QString &text, const QByteArray &id, QWidget *w, const char *slot);
    void setupActions();
    void setupToolBar();
    void setupAlternateToolBar();
    void setupUi();
    void updateUi(GuiSystem::StackedContainer *tab);

public slots:
    void onUrlChanged(const QUrl &);
    void onCurrentChanged(int);
    void onChanged();

    void onCapabilitiesChanged(GuiSystem::AbstractEditor::Capabilities capabilities);
};

} // namespace CorePlugin

#endif // BROWSERWINDOW_P_H
