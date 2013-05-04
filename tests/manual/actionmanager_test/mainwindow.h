#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace GuiSystem {
class Command;
class CommandContainer;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:
    void addTab();
    void closeTab();

private:
    void setupUi();
    void createActions();
    void createMenu();

private:
    QTabWidget *tabWidget;
    QAction *newTabAction;
    QAction *closeTabAction;
    GuiSystem::CommandContainer *menuBar;
    GuiSystem::CommandContainer *fileMenu;
    GuiSystem::CommandContainer *editMenu;
};

#endif // MAINWINDOW_H
