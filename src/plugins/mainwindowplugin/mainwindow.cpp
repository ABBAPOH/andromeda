#include "mainwindow.h"
#include "mainwindow_p.h"

#include <QtGui/QAction>
#include <QDebug>

using namespace MainWindowPlugin;

void MainWindowPrivate::onTextEntered(const QString &path)
{
    qDebug() << "MainWindowPrivate::onTextEntered" << path;
}

MainWindow::MainWindow(QWidget *parent) :
    GuiSystem::MainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->lineEdit = new EnteredLineEdit(this);
    d->toolBar = new QToolBar(this);

    QAction *backAction = new QAction(QIcon(":/images/icons/back.png"), "Back", this);
    QAction *forwardAction = new QAction(QIcon(":/images/icons/forward.png"), "Forward", this);

    d->toolBar->addAction(backAction);
    d->toolBar->addAction(forwardAction);
    d->toolBar->addSeparator();
    d->toolBar->addWidget(d->lineEdit);

    addToolBar(d->toolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(d->lineEdit, SIGNAL(textEntered(QString)), d, SLOT(onTextEntered(QString)));

    resize(640, 480);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}
