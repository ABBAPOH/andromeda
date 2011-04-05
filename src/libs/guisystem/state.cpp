#include "state.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "guicontroller.h"
#include "iview.h"
#include "iviewfactory.h"
#include "perspective.h"
#include "perspectiveinstance.h"

namespace GuiSystem {

class StatePrivate
{
    Q_DECLARE_PUBLIC(State)
public:
    StatePrivate(State *qq);
    QHash<QString, PerspectiveInstance *> instances; // perspective id -> instance
    QString currentInstanceId;
    QHash<QString, IView *> sharedViews;
    QList<IView *> views;

    void createViews(PerspectiveInstance *instance);

protected:
    State *q_ptr;
};

} // namespace GuiSystem

using namespace GuiSystem;

StatePrivate::StatePrivate(State *qq) :
    q_ptr(qq)
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
            views.append(view);
            q->connect(view, SIGNAL(destroyed(QObject*)), q, SLOT(onDestroy(QObject*)));
        } else {
            view = sharedViews.value(id);
            if (!view) {
                view = factory->view();
                views.append(view);
                q->connect(view, SIGNAL(destroyed(QObject*)), q, SLOT(onDestroy(QObject*)));
                sharedViews.insert(id, view);
            }
        }

        view->setOptions(perspective->viewOptions(id));
        instance->addView(view);
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
}

State::~State()
{
    Q_D(State);

    for (int i = 0; i < d->views.count(); i++) {
        IView *view = d->views[i];
        disconnect(view, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroy(QObject*)));
//        view->shutdown();
//        view->deleteLater();
        delete view;
    }

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

void State::onDestroy(QObject *object)
{
    qDebug("GuiSystem::State::onDestroy");
//    qDebug() << sender()->metaObject()->className();
//    IView *view = qobject_cast<IView *>(object);
    IView *view = (IView *)object;
//    Q_ASSERT(view);

    // TODO: remove bidlo coding
    Q_D(State);

    d->views.removeAll(view);

    IViewFactory *factory = qobject_cast<IViewFactory *>(view->parent());
    Q_ASSERT_X(factory, "State::onDestroy", "View's parent is not a factory.");

    QString key = factory->id();
//    QString key = d->sharedViews.key(view);
    d->sharedViews.remove(key);

    foreach (PerspectiveInstance *instance, d->instances) {
        instance->removeView(view);
    }
}
