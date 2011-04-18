#include "mainwindow.h"
#include "mainwindow_p.h"

using namespace MainWindowPlugin;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate)
{
    Q_D(MainWindow);

    d->lineEdit = new EnteredLineEdit(this);

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}
