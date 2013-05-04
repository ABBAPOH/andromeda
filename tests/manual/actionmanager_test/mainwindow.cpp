#include "mainwindow.h"

#include <QtGui/QAction>
#include <QtGui/QTextEdit>

#include <GuiSystem/ActionManager>
#include <GuiSystem/Command>
#include <GuiSystem/CommandContainer>
#include <GuiSystem/StandardCommands>

using namespace GuiSystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi();
    createMenu();
    createActions();
    setMenuBar(menuBar->menuBar());
}

void MainWindow::addTab()
{
    QTextEdit *textEdit = new QTextEdit(tabWidget);

    QAction *cutAction = new QAction(tr("Cut"), textEdit);
    cutAction->setObjectName(QLatin1String("Cut"));
    cutAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    textEdit->addAction(cutAction);
    connect(cutAction, SIGNAL(triggered()), textEdit, SLOT(cut()));

    QAction *copyAction = new QAction(tr("Copy"), textEdit);
    copyAction->setObjectName(QLatin1String("Copy"));
    copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    textEdit->addAction(copyAction);
    connect(copyAction, SIGNAL(triggered()), textEdit, SLOT(copy()));

    int index = tabWidget->addTab(textEdit, tr("Tab %1").arg(tabWidget->count() + 1));
    tabWidget->setCurrentIndex(index);
}

void MainWindow::closeTab()
{
    if (tabWidget->count() <= 1)
        return;

    int index = tabWidget->currentIndex();
    QWidget *tab = tabWidget->widget(index);
    tabWidget->removeTab(index);
    tab->deleteLater();
}

void MainWindow::setupUi()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    setCentralWidget(tabWidget);
}

void MainWindow::createActions()
{
    newTabAction = new QAction(tr("New tab"), this);
    newTabAction->setObjectName("NewTab");
    addAction(newTabAction);
    connect(newTabAction, SIGNAL(triggered()), SLOT(addTab()));

    closeTabAction = new QAction(tr("Close tab"), this);
    closeTabAction->setObjectName("Close");
    addAction(closeTabAction);
    connect(closeTabAction, SIGNAL(triggered()), SLOT(closeTab()));
}

void MainWindow::createMenu()
{
    menuBar = new CommandContainer("MenuBar", this);
    fileMenu = new CommandContainer("FileMenu", this);
    fileMenu->setText(tr("File"));

    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::NewTab));
    fileMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Close));
    menuBar->addCommand(fileMenu);

    editMenu = new CommandContainer("EditMenu", this);
    editMenu->setText(tr("Edit"));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Cut));
    editMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Copy));
    menuBar->addCommand(editMenu);
}
