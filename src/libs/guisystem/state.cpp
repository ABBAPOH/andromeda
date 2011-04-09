#include "state.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "guicontroller.h"
#include "iview.h"
#include "iviewfactory.h"
#include "perspective.h"
#include "perspectiveinstance.h"
#include "mainwindow.h"

namespace GuiSystem {

class StatePrivate
{
    Q_DECLARE_PUBLIC(State)
protected:
    State *q_ptr;

public:
    StatePrivate(State *qq);
    QHash<QString, PerspectiveInstance *> instances; // perspective id -> instance
    QString currentInstanceId;
    QHash<QString, IView *> sharedViews;
    MainWindow *window;

    void createViews(PerspectiveInstance *instance);
};

} // namespace GuiSystem

using namespace GuiSystem;

StatePrivate::StatePrivate(State *qq) :
    q_ptr(qq),
    window(0)
{
}

void StatePrivate::createViews(PerspectiveInstance *instance)
{
    Q_Q(State);

    Perspective *perspective = instance->perspective();
    GuiController *controller = GuiController::instance();

    QStringList ids = perspective->views();
    for (int i = 0; i < ids.size(); i++) {
        QString id = ids[i];
        IViewFactory *factory = controller->factory(id);
        if (!factory) {
            qWarning() << "Can't find view with id" << id;
            continue;
        }

        IView *view = 0;
        if (factory->shareMode() == IViewFactory::Clone) {
            view = factory->view();
//            views.append(view);
        } else {
            view = sharedViews.value(id);
            if (!view) {
                view = factory->view();
//                views.append(view);
                sharedViews.insert(id, view);
            }
        }

        view->setOptions(perspective->viewOptions(id));
        instance->addView(view);
        QWidget *container = q->window()->createContainer(view, view->area());
        view->setContainer(container);
    }

    QList<IView *> instanceViews = instance->views();
    for (int i = 0; i < instanceViews.size(); i++) { // 20-years on counters market
        instanceViews[i]->initialize(q);
    }
}

State::State(QObject *parent) :
    QObject(parent),
    d_ptr(new StatePrivate(this))
{
    GuiController *conTROLLer = GuiController::instance();
    connect(conTROLLer, SIGNAL(factoryRemoved(QString)), this, SLOT(onFactoryRemoved(QString)));
}

State::~State()
{
    Q_D(State);

    foreach (PerspectiveInstance *instance, d->instances.values()) {
        QList<IView *> views = instance->views();
        qDeleteAll(views);
        delete instance;
    }

    qDeleteAll(d->sharedViews.values());

    delete d_ptr;
}

PerspectiveInstance * State::currentInstance() const
{
    Q_D(const State);

    return d->instances.value(d->currentInstanceId);
}

void State::setCurrentPerspective(const QString &id)
{
    Q_D(State);

    hideViews();
    PerspectiveInstance *instance = d->instances.value(id);
    if (!instance) {
        Perspective *perspective = GuiController::instance()->perspective(id);
        if (!perspective) {
            qWarning() << "State::setCurrentInstanceId: Can't find perspective with id" << id;
            return;
        }
        instance = new PerspectiveInstance(this);
        instance->setPerspective(perspective);
        d->createViews(instance);
        d->instances.insert(id, instance);
    }
    d->currentInstanceId = id;

    if (window()->currentState() == this) {
        // display instance
        showViews();
    }

    emit currentPerspectiveChanged(id);
}

// TODO: Implement. Need to change current instance.
//void State::closeInstance(const QString &id)
//{
//    Q_D(State);
//    delete d->instances.value(id);
//    d->instances.remove(id);
//}

QStringList State::perspectiveIds() const
{
    Q_D(const State);

    return d->instances.keys();
}

QStringList State::availablePerspectives() const
{
    return GuiController::instance()->perspectiveIds();
}

void State::onFactoryRemoved(const QString &id)
{
    qDebug("State::onFactoryRemoved");

    Q_D(State);

    IView *view = d->sharedViews.value(id);
    d->sharedViews.remove(id);
    delete view;

    foreach (PerspectiveInstance *instance, d->instances.values()) {
        IView *view = instance->view(id);
        instance->removeView(id);
        delete view;
    }
}

MainWindow * State::window() const
{
    return d_func()->window;
}

void State::setWindow(MainWindow *window)
{
    d_func()->window = window;
}

void State::hideViews()
{
//    Q_D(State);

    if (!currentInstance())
        return;

    foreach (IView *view, currentInstance()->views()) {
        view->container()->hide();
    }
}

void State::showViews()
{
    foreach (IView *view, currentInstance()->views()) {
        view->container()->show();
    }
}

