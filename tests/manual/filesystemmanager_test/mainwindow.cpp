#include "mainwindow.h"

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QMenuBar>
#include <FilemanagerPlugin>

using namespace FileManagerPlugin;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QDockWidget * dock = new QDockWidget();

    NavigationPanel * panel = new NavigationPanel(dock);
    dock->setWidget(panel);
    dock->setTitleBarWidget(new QWidget); // remove TitleBar

    addDockWidget(Qt::LeftDockWidgetArea, dock);

    model = new QFileSystemModel(this);
    model->setRootPath("/");
    model->setFilter(QDir::AllDirs | QDir::System | QDir::Hidden | QDir::Files /*| QDir::NoDotAndDotDot*/);

    view = new FileManagerWidget(this);

    setCentralWidget(view);

    connect(panel, SIGNAL(folderClicked(QString)), this, SLOT(onClick(QString)));
    connect(view, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClick(QModelIndex)));

    QMenu * menu = menuBar()->addMenu("Edit");
    menu->addAction("Undo", view, SLOT(undo()), tr("Ctrl+Z"));
    menu->addAction("Redo", view, SLOT(redo()), tr("Ctrl+Shift+Z"));
    menu->addSeparator();
    menu->addAction("Copy", view, SLOT(copy()), tr("Ctrl+C"));
    menu->addAction("Paste", view, SLOT(paste()), tr("Ctrl+V"));
    menu->addAction("Remove", view, SLOT(remove()), tr("Ctrl+Shift+Backspace"));

    resize(640, 480);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onClick(const QString & path)
{
    view->setCurrentPath(path);
}

void MainWindow::onDoubleClick(const QModelIndex &index)
{
    QString path = model->filePath(index);
    path = QFileInfo(path).canonicalFilePath();
    view->setCurrentPath(path);
}
