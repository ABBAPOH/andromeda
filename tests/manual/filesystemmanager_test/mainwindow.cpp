#include "mainwindow.h"

#include <QAction>
#include <QMenu>
#include <QDockWidget>
#include <QTreeWidget>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QMenuBar>

#include <FileManager/NavigationPanel>
#include <FileManager/FileManagerWidget>
#include <FileManager/FileExplorerWidget>

using namespace FileManager;

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

    widget = new FileManagerWidget(this);
    setCentralWidget(widget);

    connect(panel, SIGNAL(currentPathChanged(QString)), widget, SLOT(setCurrentPath(QString)));

    QMenu * menu = menuBar()->addMenu("Edit");
    menu->addAction("Undo", widget, SLOT(undo()), tr("Ctrl+Z"));
    menu->addAction("Redo", widget, SLOT(redo()), tr("Ctrl+Shift+Z"));
    menu->addSeparator();
    menu->addAction("Copy", widget, SLOT(copy()), tr("Ctrl+C"));
    menu->addAction("Paste", widget, SLOT(paste()), tr("Ctrl+V"));
    menu->addSeparator();
    menu->addAction("Remove", widget, SLOT(remove()), tr("Ctrl+Shift+Backspace"));

    menu->addAction("Undo", widget, SLOT(undo()), tr("Ctrl+Z"));
    menu->addAction("Redo", widget, SLOT(redo()), tr("Ctrl+Shift+Z"));
    menu->addSeparator();
    menu->addAction("Remove", widget, SLOT(remove()), tr("Ctrl+Shift+Backspace"));

    resize(1024, 640);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onClick(const QString & path)
{
    widget->setCurrentPath(path);
}

void MainWindow::onDoubleClick(const QModelIndex &index)
{
    QString path = model->filePath(index);
    path = QFileInfo(path).canonicalFilePath();
    widget->setCurrentPath(path);
}
