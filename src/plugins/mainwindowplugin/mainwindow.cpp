#include "mainwindow.h"
#include "mainwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtGui/QAction>
#include <QtGui/QDesktopServices>
#include <QDebug>
#include <CorePlugin>
#include <page.h>

#include <iview.h>

using namespace MainWindowPlugin;
using namespace CorePlugin;
//using namespace GuiSystem;

void MainWindowPrivate::onTextEntered(const QString &path)
{
    Q_Q(MainWindow);

    Page *page = q->currentState()->object<Page>("page");
    Q_ASSERT(page);
    page->setCurrentPath(path);
}

MainWindow::MainWindow(QWidget *parent) :
    GuiSystem::MainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->lineEdit = new EnteredLineEdit(this);
    d->toolBar = new QToolBar(this);

    Page * page = new Page;
    currentState()->addObject(page, "page");
    connect(page, SIGNAL(currentPathChanged(QString)), d->lineEdit, SLOT(setText(QString)));

    QAction *backAction = new QAction(QIcon(":/images/icons/back.png"), "Back", this);
    QAction *forwardAction = new QAction(QIcon(":/images/icons/forward.png"), "Forward", this);

    d->toolBar->addAction(backAction);
    d->toolBar->addAction(forwardAction);
    d->toolBar->addSeparator();
    d->toolBar->addWidget(d->lineEdit);

    addToolBar(d->toolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(d->lineEdit, SIGNAL(textEntered(QString)), d, SLOT(onTextEntered(QString)));
    connect(backAction, SIGNAL(triggered()), SLOT(back()));
    connect(forwardAction, SIGNAL(triggered()), SLOT(forward()));

    resize(640, 480);
    page->setCurrentPath(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

void MainWindow::back()
{
    Page *page = currentState()->object<Page>("page");
    Q_ASSERT(page);
    page->history()->back();
}

void MainWindow::forward()
{
    Page *page = currentState()->object<Page>("page");
    Q_ASSERT(page);
    page->history()->forward();
}
