#include "mainwindow.h"
#include "mainwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtGui/QAction>
#include <QtGui/QDesktopServices>

//#include <QtGui/QFileSystemModel>
#include <QtGui/QDirModel>
#include <QtGui/QCompleter>

#include <CorePlugin>

#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>
#include <iview.h>
#include <perspectivewidget.h>

using namespace CorePlugin;
using namespace GuiSystem;

Tab * MainWindowPrivate::addTab(int *index)
{
    Tab *tab = new Tab(tabWidget);
    connect(tab, SIGNAL(currentUrlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
    connect(tab, SIGNAL(changed()), SLOT(onChanged()));
    int i = tabWidget->addTab(tab, "tab");
    if (index)
        *index = i;

    return tab;
}

void MainWindowPrivate::createAction(QAction *&action, const QString &text, const QByteArray &id, QWidget *w, const char *slot)
{
    ActionManager *actionManager = ActionManager::instance();

    action = new QAction(this);
    action->setText(text);
    connect(action, SIGNAL(triggered()), w, slot);
    w->addAction(action);
    actionManager->registerAction(action, id);
}

void MainWindowPrivate::setupActions()
{
    Q_Q(MainWindow);

    ActionManager *actionManager = ActionManager::instance();

    newTabAction = new QAction(this);
    connect(newTabAction, SIGNAL(triggered()), q, SLOT(newTab()));
    q->addAction(newTabAction);
    actionManager->registerAction(newTabAction, Constants::Actions::NewTab);

    closeTabAction = new QAction(this);
    connect(closeTabAction, SIGNAL(triggered()), q, SLOT(closeTab()));
    q->addAction(closeTabAction);
    actionManager->registerAction(closeTabAction, Constants::Actions::CloseTab);

    // LineEdit
    createAction(undoAction, tr("Undo"), Constants::Actions::Undo, lineEdit, SLOT(undo()));
    createAction(redoAction, tr("Redo"), Constants::Actions::Redo, lineEdit, SLOT(redo()));

    QAction *a = new QAction(this);
    a->setSeparator(true);
    lineEdit->addAction(a);

    createAction(cutAction, tr("Cut"), Constants::Actions::Cut, lineEdit, SLOT(cut()));
    createAction(copyAction, tr("Copy"), Constants::Actions::Copy, lineEdit, SLOT(copy()));
    createAction(pasteAction, tr("Paste"), Constants::Actions::Paste, lineEdit, SLOT(paste()));
    createAction(selectAllAction, tr("Select All"), Constants::Actions::SelectAll, lineEdit, SLOT(selectAll()));

    // ToolBar
    backAction = new QAction(QIcon(":/images/icons/back.png"), tr("Back"), this);
    connect(backAction, SIGNAL(triggered()), q, SLOT(back()));
    q->addAction(backAction);
    actionManager->registerAction(backAction, Constants::Actions::Back);

    forwardAction = new QAction(QIcon(":/images/icons/forward.png"), tr("Forward"), this);
    connect(forwardAction, SIGNAL(triggered()), q, SLOT(forward()));
    q->addAction(forwardAction);
    actionManager->registerAction(forwardAction, Constants::Actions::Forward);

    upAction = new QAction(QIcon(":/images/icons/up.png"), tr("Up one level"), this);
    connect(upAction, SIGNAL(triggered()), q, SLOT(up()));
    q->addAction(upAction);
    actionManager->registerAction(upAction, Constants::Actions::Up);

    nextTabAction = new QAction(this);
#ifdef Q_OS_MAC
    nextTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Right")));
#else
    nextTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Tab")));
#endif
    nextTabAction->setShortcutContext(Qt::WindowShortcut);
    connect(nextTabAction, SIGNAL(triggered()), q, SLOT(nextTab()));
    q->addAction(nextTabAction);

    prevTabAction = new QAction(q);
#ifdef Q_OS_MAC
    prevTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Left")));
#else
    prevTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+Tab")));
#endif
    prevTabAction->setShortcutContext(Qt::WindowShortcut);
    connect(prevTabAction, SIGNAL(triggered()), q, SLOT(prevTab()));
    q->addAction(prevTabAction);
}

void MainWindowPrivate::setupToolBar()
{
    Q_Q(MainWindow);

    toolBar = new QToolBar(q);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    toolBar->addAction(backAction);
    toolBar->addAction(forwardAction);
    toolBar->addAction(upAction);

    toolBar->addSeparator();
    toolBar->addWidget(lineEdit);

    q->addToolBar(toolBar);
    q->setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindowPrivate::setupAlternateToolBar()
{
    Q_Q(MainWindow);

    CommandContainer *c = ActionManager::instance()->container(Constants::Objects::AlternateToolbar);
    if (!c)
        return;

    QToolBar *toolBar = c->toolBar();

    if (!toolBar)
        return;

#if defined(Q_WS_MAC)
    QWidget *centralWidget = new QWidget(q);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFrame *frame = new QFrame(q);
    QPalette framePalette = frame->palette();
    framePalette.setColor(QPalette::Active, QPalette::Light, QColor(64, 64, 64));
    framePalette.setColor(QPalette::Active, QPalette::Dark, QColor(192, 192, 192));
    framePalette.setColor(QPalette::Inactive, QPalette::Light, QColor(135, 135, 135));
    framePalette.setColor(QPalette::Inactive, QPalette::Dark, QColor(226, 226, 226));

    frame->setLineWidth(1);
    frame->setMidLineWidth(0);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    frame->setAttribute(Qt::WA_MacNoClickThrough, true);
    frame->setPalette(framePalette);

    QPalette toolBarPalette = toolBar->palette();
    toolBarPalette.setColor(QPalette::Active, QPalette::Window, QColor(167, 167, 167));
    toolBarPalette.setColor(QPalette::Inactive, QPalette::Window, QColor(207, 207, 207));

    toolBar->setAutoFillBackground(true);
    toolBar->setFixedHeight(19);
    toolBar->setAttribute(Qt::WA_MacNoClickThrough, true);
    toolBar->setBackgroundRole(QPalette::Window);
    toolBar->setPalette(toolBarPalette);

    layout->addWidget(frame);
    layout->addWidget(toolBar);
    layout->addWidget(tabWidget);
    q->setCentralWidget(centralWidget);
#else
    q->addToolBarBreak();
    q->addToolBar(alternateToolBar);
#endif
}

void MainWindowPrivate::setupUi()
{
    Q_Q(MainWindow);

    newTabButton = new TabBarButton();
    newTabButton->setIcon(QIcon(":/images/icons/addtab.png"));
    newTabButton->setIconSize(QSize(32,32));
    connect(newTabButton, SIGNAL(clicked()), q, SLOT(newTab()));

    tabWidget = new MyTabWidget;
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    tabWidget->setUsesScrollButtons(true);
    tabWidget->setCornerWidget(newTabButton);

    q->setCentralWidget(tabWidget);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), q, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(tabBarDoubleClicked()), q, SLOT(newTab()));

    lineEdit = new EnteredLineEdit(q);
    lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
    lineEdit->setStyleSheet("QLineEdit { border-radius: 2px; }");
    connect(lineEdit, SIGNAL(textEntered(QString)), this, SLOT(onUserInput(QString)));

// ### fixme QDirModel is used in QCompleter because QFileSystemModel seems broken
// This is an example how to use completers to help directory listing.
// I'm not sure if it shouldn't be a part of plugins (standalone for web...)
// TODO/FIXME: QFileSystemModel is probably broken for qcompleter so the obsolete
//             QDirModel is used here.
//    QFileSystemModel * dirModel = new QFileSystemModel(this);
    QDirModel *dirModel = new QDirModel(this);
//    dirModel->setRootPath(QDir::rootPath());
    QCompleter *completer = new QCompleter(dirModel, lineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    lineEdit->setCompleter(completer);

    q->resize(800, 600);
}

void MainWindowPrivate::updateUi(Tab *tab)
{
    Q_Q(MainWindow);

    int index = tabWidget->indexOf(tab);
    tabWidget->setTabText(index, tab->title());
#ifndef Q_OS_MAC
    tabWidget->setTabIcon(index, tab->icon());
#endif

    if (tab == q->currentTab()) {
        q->setWindowTitle(QString(tr("%1 - Andromeda").arg(tab->windowTitle())));
        q->setWindowIcon(tab->icon());
    }
}

void MainWindowPrivate::onUserInput(const QString &userInput)
{
    q_func()->open(QUrl::fromUserInput(userInput));
}

static inline QString urlToUserOputput(const QUrl &url)
{
    if (url.scheme() == QLatin1String("file"))
        return url.toLocalFile();
    else if (url.scheme() == qApp->applicationName())
        return QString();
    else
        return url.toString();
}

void MainWindowPrivate::onUrlChanged(const QUrl &url)
{
    Q_Q(MainWindow);

    if (sender() == q->currentTab())
        lineEdit->setText(urlToUserOputput(url));
}

void MainWindowPrivate::onCurrentChanged(int index)
{
    Tab *tab = qobject_cast<Tab *>(tabWidget->widget(index));
    if (!tab)
        return;

    lineEdit->setText(urlToUserOputput(tab->currentUrl()));

    updateUi(tab);
}

void MainWindowPrivate::onChanged()
{
    Tab *tab = qobject_cast<Tab *>(sender());
    if (!tab)
        return;

    updateUi(tab);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->setupUi();
    d->setupActions();
    d->setupToolBar();
    d->setupAlternateToolBar();

    setMenuBar(ActionManager::instance()->container(Constants::Menus::MenuBar)->menuBar());
    setAttribute(Qt::WA_DeleteOnClose);

    QVariant value = Core::instance()->settings()->value(QLatin1String("mainWindow/geometry"));
//    if (value.isValid())
//        restoreGeometry(value.toByteArray());
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

int MainWindow::currentIndex() const
{
    return d_func()->tabWidget->currentIndex();
}

Tab * MainWindow::currentTab() const
{
    return qobject_cast<Tab *>(d_func()->tabWidget->currentWidget());
}

int MainWindow::count() const
{
    return d_func()->tabWidget->count();
}

void MainWindow::restoreSession(QSettings &s)
{
    Q_D(MainWindow);

#ifdef Q_OS_MAC // QTBUG-3116
    QRect geometry = s.value(QLatin1String("geometry")).toRect();
    geometry.setHeight(geometry.height() + 38); // 38 is toolbar height. i hope:(
    setGeometry(geometry);
#else
    setGeometry(s.value(QLatin1String("geometry")).toRect());
#endif
    restoreState(s.value(QLatin1String("state")).toByteArray());

    int tabCount = s.beginReadArray(QLatin1String("tabs"));
    for (int i = 0; i < tabCount; i++) {
        s.setArrayIndex(i);

        Tab *tab = d->addTab();
        tab->restoreSession(s);
    }
    s.endArray();

    d->tabWidget->setCurrentIndex(s.value(QLatin1String("currentTab")).toInt());
}

void MainWindow::saveSession(QSettings &s)
{
    Q_D(MainWindow);

    s.setValue(QLatin1String("geometry"), geometry());
    s.setValue(QLatin1String("state"), saveState());
    s.setValue(QLatin1String("currentTab"), d->tabWidget->currentIndex());

    int tabCount = d->tabWidget->count();
    s.beginWriteArray(QLatin1String("tabs"), tabCount);
    for (int i = 0; i < tabCount; i++) {
        Tab *tab = static_cast<Tab*>(d->tabWidget->widget(i));
        s.setArrayIndex(i);
        tab->saveSession(s);
    }
    s.endArray();
}

AbstractEditor * MainWindow::currentEditor() const
{
    return currentTab()->currentEditor();
}

MainWindow * MainWindow::currentWindow()
{
    return qobject_cast<MainWindow *>(qApp->activeWindow());
}

QList<MainWindow *> MainWindow::windows()
{
    QList<MainWindow*> result;
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        MainWindow* window = qobject_cast<MainWindow*>(widget);
        if (window)
            result.append(window);
    }
    return result;
}

MainWindow * MainWindow::createWindow()
{
    return new MainWindow();
}

void MainWindow::back()
{
    currentTab()->history()->back();
}

void MainWindow::forward()
{
    currentTab()->history()->forward();
}

void MainWindow::up()
{
    currentTab()->up();
}

void MainWindow::open(const QUrl &url)
{
    Q_D(MainWindow);

    if (d->tabWidget->count() == 0)
        openNewTab(url);
    else
        currentTab()->open(url);
}

void MainWindow::openEditor(const QString &id)
{
    Q_D(MainWindow);

    if (d->tabWidget->count() == 0) {
        int index = -1;
        Tab *tab = d->addTab(&index);
        tab->openEditor(id);
        d->tabWidget->setCurrentIndex(index);

        if (!tab->currentEditor())
            closeTab(index); // close tab or window if no editor found
    } else {
        currentTab()->openEditor(id);
    }
}

void MainWindow::openNewTab(const QUrl &url)
{
    Q_D(MainWindow);

    int index = -1;
    Tab *tab = d->addTab(&index);
    tab->open(url);
    d->tabWidget->setCurrentIndex(index);

    if (!tab->currentEditor())
        closeTab(index); // close tab or window if no editor found
}

void MainWindow::openNewWindow(const QUrl &path)
{
    MainWindow *window = createWindow();
    window->open(path);
    window->show();
}

void MainWindow::newTab()
{
    openNewTab(QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)));
}

void MainWindow::newWindow()
{
    openNewWindow(QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)));
}

void MainWindow::closeTab(int index)
{
    Q_D(MainWindow);

    if (d->tabWidget->count() <= 1) {
        close();
        return;
    }

    if (index == -1) {
        index = d->tabWidget->currentIndex();
    }

    if (index == d->tabWidget->currentIndex())
        d->tabWidget->setCurrentIndex(index ? index - 1 : d->tabWidget->count() - 1); // switch to other tab before closing to prevent losing focus

    QWidget *widget = d->tabWidget->widget(index);
    d->tabWidget->removeTab(index);
    widget->deleteLater();
}

void MainWindow::nextTab()
{
    Q_D(MainWindow);

    int index = d->tabWidget->currentIndex();
    d->tabWidget->setCurrentIndex(index == d->tabWidget->count() - 1 ? 0 : index + 1);
}

void MainWindow::prevTab()
{
    Q_D(MainWindow);

    int index = d->tabWidget->currentIndex();
    d->tabWidget->setCurrentIndex(index ? index - 1 : d->tabWidget->count() - 1);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    Core::instance()->settings()->setValue(QLatin1String("mainWindow/geometry"), saveGeometry());
}
