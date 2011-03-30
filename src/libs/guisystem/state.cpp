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
    QHash<QString, PerspectiveInstance*> instances; // perspective id -> instance
    QString currentInstanceId;

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

        IView *view = factory->view(q);
        view->setOptions(perspective->viewOptions(id));
        instance->addView(view);
    }

    QList<IView *> views = instance->views();
    for (int i = 0; i < views.size(); i++) { // 20-years on counters market
        views[i]->initialize(q);
    }
}

State::State(QObject *parent) :
    QObject(parent),
    d_ptr(new StatePrivate(this))
{
}

State::~State()
{
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
