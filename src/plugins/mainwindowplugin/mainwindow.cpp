#include "mainwindow.h"

#include <enteredlineedit.h>

namespace MainWindowPlugin {

class MainWindowPrivate
{
public:
};

} // namespace MainWindowPlugin

using namespace CorePlugin;
using namespace MainWindowPlugin;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate)
{
    Q_D(MainWindow);

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}
