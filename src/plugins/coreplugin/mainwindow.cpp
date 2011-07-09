#include "mainwindow.h"
#include "mainwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtGui/QAction>
#include <QtGui/QDesktopServices>
#include <QDebug>
#include <CorePlugin>

#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>
#include <iview.h>
#include <perspectivewidget.h>

using namespace CorePlugin;
using namespace GuiSystem;

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

void MainWindowPrivate::onDisplayNameChanged(const QString &name)
{
    Tab *tab = qobject_cast<Tab *>(sender());
    if (!tab)
        return;

    int index = tabWidget->indexOf(tab);
    tabWidget->setTabText(index, name);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->tabWidget = new /*QTabWidget*/MyTabWidget;
    d->tabWidget->setDocumentMode(true);
    d->tabWidget->setTabsClosable(true);
    setCentralWidget(d->tabWidget);

    d->lineEdit = new EnteredLineEdit(this);
    d->lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
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

    QAction *backAction = actionManager->command(Constants::Ids::Actions::Back)->action();
    backAction->setIcon(QIcon(":/images/icons/back.png"));
    addAction(backAction);

    QAction *forwardAction = actionManager->command(Constants::Ids::Actions::Forward)->action();
    forwardAction->setIcon(QIcon(":/images/icons/forward.png"));
    addAction(forwardAction);

    d->toolBar->addAction(backAction);
    d->toolBar->addAction(forwardAction);
//    d->toolBar->addAction(actionManager->command(Constants::Ids::Actions::Up)->commandAction());
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

    resize(640, 480);
    newTab();
}

MainWindow::~MainWindow()
{
    delete d_ptr;
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

    Tab *tab = new Tab(d->tabWidget);
    connect(tab, SIGNAL(currentPathChanged(QString)), d->lineEdit, SLOT(setText(QString)));
    connect(tab, SIGNAL(displayNameChanged(QString)), d, SLOT(onDisplayNameChanged(QString)));
    int index = d->tabWidget->addTab(tab, "tab");
    tab->setCurrentPath(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    d->tabWidget->setCurrentIndex(index);
}

void MainWindow::closeTab(int index)
{
    Q_D(MainWindow);

    if (d->tabWidget->count() <= 1) {
        close();
        deleteLater();
        return;
    }

    if (index == -1)
        index = d->tabWidget->currentIndex();
    QWidget *widget = d->tabWidget->widget(index);
    d->tabWidget->removeTab(index);
    widget->deleteLater();
}
