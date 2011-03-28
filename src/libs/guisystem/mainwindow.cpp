#include "mainwindow.h"

#include "iview.h"
#include "perspectiveinstance.h"

#include <QtGui/QDockWidget>
#include <QtGui/QToolBar>

#include <QDebug>

namespace GuiSystem {

class MainWindowPrivate
{
public:
    PerspectiveInstance *currentInstance;
};

} // namespace GuiSystem

using namespace GuiSystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate)
{
    resize(640, 480);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

PerspectiveInstance * MainWindow::perspectiveInstance() const
{
    return d_func()->currentInstance;
}

void MainWindow::setPerspectiveInstance(PerspectiveInstance *instance)
{
    // TODO: Delete somewhere old perspective instance
    d_func()->currentInstance = instance;

    displayInstance();
}

void MainWindow::displayInstance()
{
    Q_D(MainWindow);

    QList<IView *> views = d->currentInstance->views();
    for (int i = 0; i < views.size(); i++) {
        IView *view = views[i];
        int area = view->area();

        if (area > 0 && area < 5) {
            QDockWidget *dock = new QDockWidget;
            if (view->toolBar())
                dock->setTitleBarWidget(view->toolBar());
            dock->setWidget(view->widget());

            switch (area) {
            case 1:
                addDockWidget(Qt::LeftDockWidgetArea, dock);
                break;
            case 2:
                addDockWidget(Qt::TopDockWidgetArea, dock);
                break;
            case 3:
                addDockWidget(Qt::RightDockWidgetArea, dock);
                break;
            case 4:
                addDockWidget(Qt::BottomDockWidgetArea, dock);
                break;
            default:
                break;
            }
        } else if (area == 6) {
            // FIXME: add toolbar
            setCentralWidget(view->widget());
        }
    }
}
