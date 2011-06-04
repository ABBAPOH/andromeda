#include "mainwindow.h"

#include "actionmanager.h"
#include "centralwidget.h"
#include "iview.h"
#include "perspectiveinstance.h"

#include <QtCore/QList>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QTabBar>

#include <QDebug>

#include "state.h"

namespace GuiSystem {

class MainWindowPrivate
{
public:
    struct Data
    {
        QMap<IView *, QWidget *> mapToWidget;
    };


    PerspectiveInstance *currentInstance;

    State *currentState;
    int currentStateIndex;
    QList<State *> states;

    QVBoxLayout *layout;

    QSet<PerspectiveInstance *> instances;
    QTabBar *tabBar;
    QToolBar *toolBar;
//    QHash<IView *, QWidget *> mapToWidget;
};

} // namespace GuiSystem

using namespace GuiSystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate)
{
    Q_D(MainWindow);

    d->currentState = 0;
    d->layout = new QVBoxLayout();
    d->layout->setMargin(0);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(d->layout);

    d->tabBar = new QTabBar();
    d->tabBar->hide();
//    d->toolBar = new QToolBar;
//    addToolBar(d->toolBar);
//    d->toolBar->addWidget(d->tabBar);

//    setUnifiedTitleAndToolBarOnMac(true);
    d->tabBar->setDocumentMode(true);
    d->tabBar->setTabsClosable(true);

    d->currentInstance = 0;
    State *state = new State(this);
    addState(state);
//    d->currentStateIndex = 0;
    state->setCurrentPerspective("empty");
    setCurrentState(0);

    connect(d->tabBar, SIGNAL(currentChanged(int)), SLOT(setCurrentState(int)));
    connect(d->tabBar, SIGNAL(tabCloseRequested(int)), SLOT(onTabClose(int)));
//    connect(d->currentState, SIGNAL(currentPerspectiveChanged(QString)), SLOT(setPerspective(QString)));

//    d->centralWidget = new CentralWidget(this);
//    setCentralWidget(d->centralWidget);

//    d->tabBar->addTab("     111     ");
//    d->tabBar->addTab("222");

    ActionManager *manager = ActionManager::instance();
    QList<QAction*> actions = manager->actions();
    for (int i = 0; i < actions.size(); i++) {
        menuBar()->addAction(actions[i]);
    }
    connect(manager, SIGNAL(actionAdded(QAction*)), SLOT(onActionAdd(QAction*)));

    resize(640, 480);
}

MainWindow::~MainWindow()
{
//    Q_D(MainWindow);

//    delete d->currentState;

    delete d_ptr;
}

State * MainWindow::currentState() const
{
    Q_D(const MainWindow);

    if (d->states.isEmpty())
        return 0;

    return d->currentState;
}

int MainWindow::currentStateIndex() const
{
    return d_func()->currentStateIndex;
}

void MainWindow::setCurrentState(int index)
{
    Q_D(MainWindow);

    qDebug() << "setCurrentState" << index;
//    if (d->currentStateIndex != index) {
    if (d->currentState != d->states[index]) {
        if (currentState() != 0)
            currentState()->hideViews();
        d->currentStateIndex = index;
        d->currentState = d->states[index];
        currentState()->showViews();

        // TODO: split gui and non-gui logic:)
        if (d->tabBar->currentIndex() != d->currentStateIndex)
            d->tabBar->setCurrentIndex(d->currentStateIndex);
    }
}

QList<State *> MainWindow::states() const
{
    return d_func()->states;
}

void MainWindow::addState(State *state)
{
    Q_D(MainWindow);

    if (!d->states.contains(state)) {
        d->states.append(state);
        state->setWindow(this);
    }

    d->tabBar->addTab("     111     ");
}

void MainWindow::addState()
{
    State *state = new State(this);
    addState(state);
    state->setCurrentPerspective("empty");
}

void MainWindow::removeState(int index)
{
    Q_D(MainWindow);

    // TODO: last closed options
    if (d->states.count() == 1)
        return;

    d->states.removeAt(index);
    // TODO: switch to options
    if (d->currentStateIndex == index) {
        setCurrentState((index + 1) % stateCount());
    }

    d->tabBar->removeTab(index);
}

int MainWindow::stateCount() const
{
    return d_func()->states.count();
}

QWidget * MainWindow::createContainer(IView *view, int area)
{
    Q_D(MainWindow);
//    int area = view->area();

    if (area > 0 && area < 5) {
        QDockWidget *dock = new QDockWidget(this);
        if (view->toolBar())
            dock->setTitleBarWidget(view->toolBar());
        else
            dock->setTitleBarWidget(new QWidget);
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
        return dock;

    } else if (area == 6) {
        CentralWidget *widget = new CentralWidget(centralWidget());
        widget->setView(view);
        d->layout->addWidget(widget);
        return widget;
    }
    return 0;
}

void MainWindow::displayView(IView *view, int area)
{
//    Q_D(MainWindow);

    if (area > 0 && area < 5) {
        QDockWidget *dock = qobject_cast<QDockWidget *>(view->container());

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
        QWidget *widget = view->container();
        widget->show();
//        d->layout->removeWidget(widget);
//        d->layout->addWidget(widget);
    }
}

PerspectiveInstance * MainWindow::perspectiveInstance() const
{
    return d_func()->currentInstance;
}

void MainWindow::displayInstance()
{
//    Q_D(MainWindow);
//    qDebug() << " MainWindow::displayInstance()";

//    // TODO: remove old currentInstance. Use State. Check null instance.
//    if (!d->instances.contains(d->currentState->currentInstance())) {
//        d->instances.insert(d->currentState->currentInstance());
//        createWidgetsForInstance();
//    }

//    if (d->currentInstance) {
//        foreach (IView * view, d->currentInstance->views()) {
//            view->container()->hide();
//        }
//    }

//    d->currentInstance = d->currentState->currentInstance();

//    foreach (IView * view, d->currentInstance->views()) {
//        view->container()->show();
//    }
}

void MainWindow::setPerspective(const QString &id)
{
    displayInstance();
}

void MainWindow::createWidgetsForInstance()
{
//    Q_D(MainWindow);

//    PerspectiveInstance *instance = d->currentState->currentInstance();
//    QList<IView *> views = instance->views();
//    for (int i = 0; i < views.size(); i++) {
//        IView *view = views[i];
//        int area = view->area();

//        if (area > 0 && area < 5) {
//            QDockWidget *dock = new QDockWidget;
//            if (view->toolBar())
//                dock->setTitleBarWidget(view->toolBar());
//            dock->setWidget(view->widget());

//            view->setContainer(dock);

//            switch (area) {
//            case 1:
//                addDockWidget(Qt::LeftDockWidgetArea, dock);
//                break;
//            case 2:
//                addDockWidget(Qt::TopDockWidgetArea, dock);
//                break;
//            case 3:
//                addDockWidget(Qt::RightDockWidgetArea, dock);
//                break;
//            case 4:
//                addDockWidget(Qt::BottomDockWidgetArea, dock);
//                break;
//            default:
//                break;
//            }
//        } else if (area == 6) {
//            // FIXME: add toolbar
//            CentralWidget *widget = new CentralWidget();
//            widget->setView(view);
//            view->setContainer(widget);
//            d->layout->addWidget(widget);
//        }
//    }
    //    d->instances.insert(instance);
}

void MainWindow::onTabClose(int index)
{
    qDebug() << "onTabClose" << index;
    removeState(index);
}

void MainWindow::onActionAdd(QAction *action)
{
    menuBar()->addAction(action);
}
