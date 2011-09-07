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
    connect(tab, SIGNAL(currentPathChanged(QString)), SLOT(onPathChanged(QString)));
    connect(tab, SIGNAL(changed()), SLOT(onChanged()));
    int i = tabWidget->addTab(tab, "tab");
    if (index)
        *index = i;

    return tab;
}

int MainWindowPrivate::currentIndex() const
{
    return tabWidget->currentIndex();
}

CorePlugin::Tab * MainWindowPrivate::currentTab() const
{
    return qobject_cast<CorePlugin::Tab *>(tabWidget->currentWidget());
}

void MainWindowPrivate::onTextEntered(const QString &path)
{
    currentTab()->setCurrentPath(path);
}

void MainWindowPrivate::onPathChanged(const QString &s)
{
    if (sender() == currentTab())
        lineEdit->setText(s);
}

void MainWindowPrivate::onChanged()
{
    Tab *tab = qobject_cast<Tab *>(sender());
    if (!tab)
        return;

    int index = tabWidget->indexOf(tab);
    tabWidget->setTabText(index, tab->title());

    q_func()->setWindowTitle(QString(tr("%1 - Andromeda").arg(tab->windowTitle())));
    q_func()->setWindowIcon(tab->icon());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->tabWidget = new MyTabWidget;
    d->tabWidget->setDocumentMode(true);
    d->tabWidget->setTabsClosable(true);
    setCentralWidget(d->tabWidget);

    d->lineEdit = new EnteredLineEdit(this);
    d->lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);

// ### fixme QDirModel is used in QCompleter because QFileSystemModel seems broken
    // This is an example how to use completers to help directory listing.
    // I'm not sure if it shouldn't be a part of plugins (standalone for web...)
    // TODO/FIXME: QFileSystemModel is probably broken for qcompleter so the obsolete
    //             QDirModel is used here.
//    QFileSystemModel * dirModel = new QFileSystemModel(this);
    QDirModel *dirModel = new QDirModel(this);
//    dirModel->setRootPath(QDir::rootPath());
    QCompleter *completer = new QCompleter(dirModel, d->lineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    d->lineEdit->setCompleter(completer);

    ActionManager *actionManager = ActionManager::instance();
    actionManager->command(Constants::Ids::Actions::Undo)->action(d->lineEdit, SLOT(undo()));
    actionManager->command(Constants::Ids::Actions::Redo)->action(d->lineEdit, SLOT(redo()));
    QAction *a = new QAction(this);
    a->setSeparator(true);
    d->lineEdit->addAction(a);
    actionManager->command(Constants::Ids::Actions::Cut)->action(d->lineEdit, SLOT(cut()));
    actionManager->command(Constants::Ids::Actions::Copy)->action(d->lineEdit, SLOT(copy()));
    actionManager->command(Constants::Ids::Actions::Paste)->action(d->lineEdit, SLOT(paste()));
    actionManager->command(Constants::Ids::Actions::SelectAll)->action(d->lineEdit, SLOT(selectAll()));

    d->toolBar = new QToolBar(this);

    QAction *backAction = actionManager->command(Constants::Ids::Actions::Back)->action(this);
    backAction->setIcon(QIcon(":/images/icons/back.png"));
    addAction(backAction);

    QAction *forwardAction = actionManager->command(Constants::Ids::Actions::Forward)->action(this);
    forwardAction->setIcon(QIcon(":/images/icons/forward.png"));
    addAction(forwardAction);

    d->toolBar->addAction(backAction);
    d->toolBar->addAction(forwardAction);
    d->toolBar->addAction(actionManager->command(Constants::Ids::Actions::Up)->commandAction());

    CommandContainer *gotoMenu = actionManager->container(Constants::Ids::Menus::GoTo);
    QSignalMapper *gotoMapper = new QSignalMapper(this);
    foreach (Command *cmd, gotoMenu->commands(Constants::Ids::MenuGroups::Locations)) {
        QAction *action = cmd->action();
        gotoMapper->setMapping(action, cmd->data().toString());
        connect(action, SIGNAL(triggered()), gotoMapper, SLOT(map()));
        action->setParent(this);
        addAction(action);
    }
    connect(gotoMapper, SIGNAL(mapped(QString)), d, SLOT(onTextEntered(QString)));

    d->toolBar->addSeparator();
    d->toolBar->addWidget(d->lineEdit);

    addToolBar(d->toolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    setMenuBar(actionManager->container(Constants::Ids::Menus::MenuBar)->menuBar());

    connect(d->lineEdit, SIGNAL(textEntered(QString)), d, SLOT(onTextEntered(QString)));
    connect(d->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(d->tabWidget, SIGNAL(tabBarDoubleClicked()), SLOT(newTab()));
    connect(backAction, SIGNAL(triggered()), SLOT(back()));
    connect(forwardAction, SIGNAL(triggered()), SLOT(forward()));

    resize(800, 600);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
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

void MainWindow::back()
{
    d_func()->currentTab()->history()->back();
}

void MainWindow::forward()
{
    d_func()->currentTab()->history()->forward();
}

void MainWindow::newTab()
{
    Q_D(MainWindow);

    int index = -1;
    Tab *tab = d->addTab(&index);
    tab->setCurrentPath(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    d->tabWidget->setCurrentIndex(index);
}

void MainWindow::closeTab(int index)
{
    Q_D(MainWindow);

    if (d->tabWidget->count() <= 1) {
        close();
        return;
    }

    if (index == -1)
        index = d->tabWidget->currentIndex();
    QWidget *widget = d->tabWidget->widget(index);
    d->tabWidget->currentWidget()->hide();
    d->tabWidget->removeTab(index);
    widget->deleteLater();
}
