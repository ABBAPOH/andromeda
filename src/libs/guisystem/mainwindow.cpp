#include "mainwindow.h"
#include "mainwindow_p.h"

#include "actionmanager.h"
#include "centralwidget.h"
#include "iview.h"
#include "perspectiveinstance.h"

#include <QtCore/QList>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabBar>

#include <QDebug>

#include "state.h"

using namespace GuiSystem;

MainWindowPrivate::MainWindowPrivate(MainWindow *qq) :
    q_ptr(qq)
{
}

int MainWindowPrivate::addWidget(int stateIndex, QWidget *widget)
{
    Q_ASSERT(stateIndex >= 0 && stateIndex < states.size());
    QStackedWidget *stack = qobject_cast<QStackedWidget *>(tabWidget->widget(stateIndex));
    int result = stack->addWidget(widget);
    stack->setCurrentIndex(result);
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->currentState = 0;
    d->tabWidget = new QTabWidget;
    d->tabWidget->setDocumentMode(true);
    setCentralWidget(d->tabWidget);
    newTab();

//    d->currentStateIndex = 0;

    connect(d->tabWidget, SIGNAL(currentChanged(int)), SLOT(setCurrentIndex(int)));
    connect(d->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));

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

void MainWindow::newTab()
{
    Q_D(MainWindow);

    int index = d->tabWidget->addTab(new QStackedWidget, QString("Tab %1").arg(d->tabWidget->count()+1));

    State *state = new State(this);
    state->setIndex(index);
    d->currentState = state;
    d->states.append(state);
    d->currentStateIndex = index;

    state->setWindow(this);
}

void MainWindow::setCurrentIndex(int index)
{
    Q_D(MainWindow);

    if (d->currentStateIndex != index) {
        d->currentStateIndex = index;
        d->currentState = d->states[index];

        if (d->tabWidget->currentIndex() != d->currentStateIndex)
            d->tabWidget->setCurrentIndex(d->currentStateIndex);
    }
}

void MainWindow::closeTab(int index)
{
    Q_D(MainWindow);

    // TODO: last closed options
    if (d->states.count() == 1)
        return;

    State *state = d->states.takeAt(index);
    delete state;

    QWidget * widget = d->tabWidget->widget(index);
    d->tabWidget->removeTab(index);
    delete widget;

    // TODO: switch to options
    if (d->currentStateIndex == index) {
        setCurrentIndex((index + 1) % stateCount());
    }
}

QList<State *> MainWindow::states() const
{
    return d_func()->states;
}

int MainWindow::stateCount() const
{
    return d_func()->states.count();
}

void MainWindow::onActionAdd(QAction *action)
{
    menuBar()->addAction(action);
}
