#include "mainwindow.h"

#include "iview.h"
#include "perspectiveinstance.h"

#include <QtCore/QList>
#include <QtGui/QDockWidget>
#include <QtGui/QToolBar>

#include <QDebug>

#include "state.h"

namespace GuiSystem {

class MainWindowPrivate
{
public:
    PerspectiveInstance *currentInstance;
    State *currentState;
    QList<QWidget *> widgets;
};

} // namespace GuiSystem

using namespace GuiSystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate)
{
    Q_D(MainWindow);

    d->currentInstance = 0;
    d->currentState = new State(this);
    connect(d->currentState, SIGNAL(currentPerspectiveChanged(QString)), SLOT(setPerspective(QString)));

    resize(640, 480);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

State * MainWindow::currentState() const
{
    return d_func()->currentState;
}

PerspectiveInstance * MainWindow::perspectiveInstance() const
{
    return d_func()->currentInstance;
}

void MainWindow::displayInstance()
{
    Q_D(MainWindow);

//    qDeleteAll(d->widgets);
//    d->widgets.clear();
    foreach(QWidget *widget, d->widgets) {
        widget->hide();
    }

    // TODO: remove old currentInstance. Use State. Check null instance.
    QList<IView *> views = d->currentState->currentInstance()->views();
    for (int i = 0; i < views.size(); i++) {
        IView *view = views[i];
        int area = view->area();

        if (area > 0 && area < 5) {
            QDockWidget *dock = new QDockWidget;
            if (view->toolBar())
                dock->setTitleBarWidget(view->toolBar());
            dock->setWidget(view->widget());

            d->widgets.append(dock);

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

void MainWindow::setPerspective(const QString &id)
{
    Q_D(MainWindow);

    displayInstance();
}
