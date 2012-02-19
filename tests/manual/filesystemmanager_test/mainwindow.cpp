#include "mainwindow.h"

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QMenuBar>

#include <filemanager/navigationpanel.h>
#include <filemanager/dualpanewidget.h>

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

    dualPane = new DualPaneWidget(this);
    dualPane->setDualPaneModeEnabled(true);

    setCentralWidget(dualPane);

    connect(panel, SIGNAL(currentPathChanged(QString)), dualPane, SLOT(setCurrentPath(QString)));

    QMenu * menu = menuBar()->addMenu("Edit");
    menu->addAction("Undo", view, SLOT(undo()), tr("Ctrl+Z"));
    menu->addAction("Redo", view, SLOT(redo()), tr("Ctrl+Shift+Z"));
    menu->addSeparator();
    menu->addAction("Copy", view, SLOT(copy()), tr("Ctrl+C"));
    menu->addAction("Paste", view, SLOT(paste()), tr("Ctrl+V"));
    menu->addSeparator();
    menu->addAction("Remove", view, SLOT(remove()), tr("Ctrl+Shift+Backspace"));

    menu->addAction("Undo", dualPane, SLOT(undo()), tr("Ctrl+Z"));
    menu->addAction("Redo", dualPane, SLOT(redo()), tr("Ctrl+Shift+Z"));
    menu->addSeparator();
    menu->addAction("Remove", dualPane, SLOT(remove()), tr("Ctrl+Shift+Backspace"));

    menu = menuBar()->addMenu("Tools");
    menu->addAction("Copy Files", dualPane, SLOT(copy()), tr("Ctrl+Shift+C"));
    menu->addAction("Move Files", dualPane, SLOT(move()), tr("Ctrl+Alt+C"));

    resize(1024, 640);
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
