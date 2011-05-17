#include "state.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtGui/QAction>

#include "actionmanager.h"
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
    QMultiMap<QString, QObject*> mapToObject;

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
            view->setState(q);
        } else {
            view = sharedViews.value(id);
            if (!view) {
                view = factory->view();
                sharedViews.insert(id, view);
                view->setState(q);
            }
        }

        view->setOptions(perspective->viewOptions(id));
        instance->addView(view);
        QWidget *container = q->window()->createContainer(view, view->area());
        view->setContainer(container);
    }

    QList<IView *> instanceViews = instance->views();
    for (int i = 0; i < instanceViews.size(); i++) { // 20-years on counters market
        instanceViews[i]->initialize();
    }
}

State::State(QObject *parent) :
    QObject(parent),
    d_ptr(new StatePrivate(this))
{
    GuiController *controller = GuiController::instance();
    connect(controller, SIGNAL(factoryRemoved(QString)), this, SLOT(onFactoryRemoved(QString)));
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
    d->currentInstanceId = id;
    if (!instance) {
        Perspective *perspective = GuiController::instance()->perspective(id);
        if (!perspective) {
            qWarning() << "State::setCurrentInstanceId: Can't find perspective with id" << id;
            return;
        }
        instance = new PerspectiveInstance(this);
        instance->setPerspective(perspective);
        d->instances.insert(id, instance);
        d->createViews(instance);
    }

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

void State::hideViews()
{
    if (!currentInstance())
        return;

    ActionManager *manager = ActionManager::instance();
    foreach (IView *view, currentInstance()->views()) {
        QActionList actions = manager->actions(view->factoryId());
        foreach (QAction *action, actions) {
            action->setEnabled(false);
        }
        view->container()->hide();
    }
}

void State::showViews()
{
    ActionManager *manager = ActionManager::instance();
    foreach (IView *view, currentInstance()->views()) {
        QActionList actions = manager->actions(view->factoryId());
        foreach (QAction *action, actions) {
            action->setEnabled(true);
        }
        view->container()->show();
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

void State::setProperty(const char *name, const QVariant &value)
{
    QObject::setProperty(name, value);
    emit propertyChanged(name, value);
}

IView * State::view(const QString &id)
{
    return currentInstance()->view(id);
}

/*!
    \fn void State::addObject(QObject *object, const QString &name)
    \brief Adds \a object with \a name to State's pool.

    Objects can be added to State pool and be accessed by their names. When adding object, State
    becomes it's parent. If second parameter is present, also object's name is changed to \a name.
    If second parameter is omitted, QObject::objectName() will be used as key for access to this
    object. If \a object has no name (i.e. QObject::objectName() is empty), object's class name
    (QMetaObject::className()) will be used instead as key.

    If you try to add object with name that already present in map, the first one will no be replaced.

    \sa State::object
*/
void State::addObject(QObject *object, const QString &name)
{
    Q_D(State);

    if (!object)
        return;

    if (name.isEmpty()) {
        QString newName = object->objectName();
        if (newName.isEmpty()) {
            newName = QString::fromAscii(object->metaObject()->className());
            object->setObjectName(newName);
        }
        d->mapToObject.insert(newName, object);
    } else {
        object->setObjectName(name);
        d->mapToObject.insert(name, object);
    }
    object->setParent(this);
}

/*!
    \fn void State::removeObject(QObject *object)
    \brief Removes \a object from pool.

    Note, that State still will be parent of \a object.
*/
void State::removeObject(QObject *object)
{
    Q_D(State);

    if (!object)
        return;

    QStringList keys = d->mapToObject.keys(object);
    foreach (const QString &key, keys) {
        d->mapToObject.remove(key, object);
    }
}

/*!
    \fn QObject * State::object(const QString &name) const
    \brief Returns object with \a name.

    Returns object previuosly added to pool. Returns 0 if no object has such name.

    \sa State::addObject
*/
QObject * State::object(const QString &name) const
{
    Q_D(const State);

    return d->mapToObject.value(name);
}

/*!
    \fn QObjectList State::objects(const QString &name) const
    \brief Returns list of objects with \a name.

    Returns objects previuosly added to pool. Returns empty list if no object has such name.

    \sa State::addObject
*/
QObjectList State::objects(const QString &name) const
{
    Q_D(const State);

    return d->mapToObject.values(name);
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
