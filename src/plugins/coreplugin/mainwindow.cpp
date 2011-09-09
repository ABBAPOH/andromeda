#include "mainwindow.h"
#include "mainwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtGui/QAction>
#include <QtGui/QApplication>
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
    connect(tab, SIGNAL(currentPathChanged(QString)), SLOT(onPathChanged(QString)));
    connect(tab, SIGNAL(changed()), SLOT(onChanged()));
    int i = tabWidget->addTab(tab, "tab");
    if (index)
        *index = i;

    return tab;
}

void MainWindowPrivate::setupActions()
{
    Q_Q(MainWindow);

    ActionManager *actionManager = ActionManager::instance();

    actionManager->command(Constants::Ids::Actions::NewTab)->action(q, SLOT(newTab()));
    actionManager->command(Constants::Ids::Actions::CloseTab)->action(q, SLOT(closeTab()));

    // LineEdit
    actionManager->command(Constants::Ids::Actions::Undo)->action(lineEdit, SLOT(undo()));
    actionManager->command(Constants::Ids::Actions::Redo)->action(lineEdit, SLOT(redo()));

    QAction *a = new QAction(this);
    a->setSeparator(true);
    lineEdit->addAction(a);

    actionManager->command(Constants::Ids::Actions::Cut)->action(lineEdit, SLOT(cut()));
    actionManager->command(Constants::Ids::Actions::Copy)->action(lineEdit, SLOT(copy()));
    actionManager->command(Constants::Ids::Actions::Paste)->action(lineEdit, SLOT(paste()));
    actionManager->command(Constants::Ids::Actions::SelectAll)->action(lineEdit, SLOT(selectAll()));

    // ToolBar
    backAction = actionManager->command(Constants::Ids::Actions::Back)->action(this);
    backAction->setIcon(QIcon(":/images/icons/back.png"));
    connect(backAction, SIGNAL(triggered()), q, SLOT(back()));
    q->addAction(backAction);

    forwardAction = actionManager->command(Constants::Ids::Actions::Forward)->action(this);
    forwardAction->setIcon(QIcon(":/images/icons/forward.png"));
    connect(forwardAction, SIGNAL(triggered()), q, SLOT(forward()));
    q->addAction(forwardAction);

    upAction = actionManager->command(Constants::Ids::Actions::Up)->commandAction();

    // GoTo menu
    CommandContainer *gotoMenu = actionManager->container(Constants::Ids::Menus::GoTo);
    QSignalMapper *gotoMapper = new QSignalMapper(this);
    foreach (Command *cmd, gotoMenu->commands(Constants::Ids::MenuGroups::Locations)) {
        QAction *action = cmd->action();
        gotoMapper->setMapping(action, cmd->data().toString());
        connect(action, SIGNAL(triggered()), gotoMapper, SLOT(map()));
        action->setParent(this);
        q->addAction(action);
    }
    connect(gotoMapper, SIGNAL(mapped(QString)), q, SLOT(open(QString)));
}

void MainWindowPrivate::setupToolBar()
{
    Q_Q(MainWindow);

    toolBar = new QToolBar(q);

    toolBar->addAction(backAction);
    toolBar->addAction(forwardAction);
    toolBar->addAction(upAction);

    toolBar->addSeparator();
    toolBar->addWidget(lineEdit);

    q->addToolBar(toolBar);
    q->setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindowPrivate::setupUi()
{
    Q_Q(MainWindow);

    tabWidget = new MyTabWidget;
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    q->setCentralWidget(tabWidget);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), q, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(tabBarDoubleClicked()), q, SLOT(newTab()));

    lineEdit = new EnteredLineEdit(q);
    lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(lineEdit, SIGNAL(textEntered(QString)), q, SLOT(open(QString)));

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

void MainWindowPrivate::onPathChanged(const QString &s)
{
    Q_Q(MainWindow);

    if (sender() == q->currentTab())
        lineEdit->setText(s);
}

void MainWindowPrivate::onCurrentChanged(int index)
{
    Tab *tab = qobject_cast<Tab *>(tabWidget->widget(index));
    if (!tab)
        return;

    lineEdit->setText(tab->currentPath());
}

void MainWindowPrivate::onChanged()
{
    Q_Q(MainWindow);

    Tab *tab = qobject_cast<Tab *>(sender());
    if (!tab)
        return;

    int index = tabWidget->indexOf(tab);
    tabWidget->setTabText(index, tab->title());

    q->setWindowTitle(QString(tr("%1 - Andromeda").arg(tab->windowTitle())));
    q->setWindowIcon(tab->icon());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->setupUi();
    d->setupActions();
    d->setupToolBar();

    setMenuBar(ActionManager::instance()->container(Constants::Ids::Menus::MenuBar)->menuBar());
    setAttribute(Qt::WA_DeleteOnClose);
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
    return qobject_cast<CorePlugin::Tab *>(d_func()->tabWidget->currentWidget());
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

void MainWindow::back()
{
    currentTab()->history()->back();
}

void MainWindow::forward()
{
    currentTab()->history()->forward();
}

void MainWindow::open(const QString &path)
{
    Q_D(MainWindow);

    if (d->tabWidget->count() == 0)
        openNewTab(path);
    else
        currentTab()->open(path);
}

void MainWindow::openNewTab(const QString &path)
{
    Q_D(MainWindow);

    int index = -1;
    Tab *tab = d->addTab(&index);
    tab->open(path);
    d->tabWidget->setCurrentIndex(index);
}

void MainWindow::openNewWindow(const QString &path)
{
    MainWindow *window = new MainWindow();
    window->open(path);
    window->show();
}

void MainWindow::newTab()
{
    openNewTab(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
}

void MainWindow::newWindow()
{
    openNewWindow(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
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

