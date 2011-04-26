#include "actionmanager.h"
#include "actionmanager_p.h"

#include "iview.h"
#include "perspectiveinstance.h"
#include "mainwindow.h"
#include "state.h"

#include <QtCore/QMetaMethod>

using namespace GuiSystem;

void ActionManagerPrivate::onTrigger(bool checked)
{
    qDebug() << "ActionManagerPrivate::onActionTriggered" << checked;

    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);

    // TODO: use QObject instead of QAction here
    QString id = action->property("id").toString();

    foreach (const ConnectionByObject &pair, connectionsObjects.values(id)) {
        QObject *object = pair.first;
        const char *slot = pair.second;
        QMetaObject::invokeMethod(object, slot);
    }

    MainWindow *window = qobject_cast<MainWindow *>(qApp->activeWindow());
    if (!window) {
        return;
    }

    foreach (const ConnectionById &pair, connectionsViews.values(id)) {
        QString viewId = pair.first;
        const char *slot = pair.second;
        IView * view = window->currentState()->currentInstance()->view(viewId);
        if (!view) {
            qWarning() << "Cant find view with id" << viewId;
            continue;
        }
        QMetaObject::invokeMethod(view, slot);
    }

    QWidget *targetWidget = QApplication::focusWidget();
    foreach (const char * slot, connectionsToWidgets.values(id)) {
        const QMetaObject *metaObject = targetWidget->metaObject();
        int index = metaObject->indexOfMethod(slot);
        const QMetaMethod &metaMethod = metaObject->method(index);
        metaMethod.invoke(targetWidget);
    }
}

void ActionManagerPrivate::onFocusChange(QWidget *old, QWidget *now)
{
    foreach (const QString &id, connectionsToWidgets.keys()) {
        QAction *action = mapToAction.value(id);
        if (!action)
            continue;
        const QMetaObject *metaObject = now ? now->metaObject() : 0;
        if (metaObject && metaObject->indexOfSlot(connectionsToWidgets.value(id)) != -1)
            action->setEnabled(true);
        else
            action->setEnabled(false);
    }
}

ActionManager::ActionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ActionManagerPrivate)
{
    Q_D(ActionManager);

    QObject::connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
                     d, SLOT(onFocusChange(QWidget*,QWidget*)));
}

ActionManager::~ActionManager()
{
    delete d_ptr;
}

ActionManager *ActionManager::instance()
{
    static ActionManager *m_instance = new ActionManager(qApp);
    return m_instance;
}

void ActionManager::addAction(QAction *action)
{
    Q_D(ActionManager);

    if (!d->menuBarActions.contains(action)) {
        d->menuBarActions.append(action);
        emit actionAdded(action);
    }
}

QAction * ActionManager::addMenu(QMenu *menu)
{
    QAction *menuAction = menu->menuAction();
    addAction(menuAction);
    return menuAction;
}

void ActionManager::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
}

QActionList ActionManager::actions() const
{
    Q_D(const ActionManager);

    return d->menuBarActions;
}

QActionList ActionManager::actions(const QString &viewId) const
{
    Q_D(const ActionManager);

    QActionList result;
    QStringList ids = d->mapToView.values(viewId);
    for (int i = 0; i < ids.size(); i++) {
        QAction *act = action(ids[i]);
        if (act)
            result.append(act);
    }
    return result;
}

QAction * ActionManager::action(const QString &id) const
{
    Q_D(const ActionManager);

    return d->mapToAction.value(id);
}

QMenu * ActionManager::menu(const QString &id) const
{
    Q_D(const ActionManager);

    return d->mapToMenu.value(id);
}

/*!
    \fn void ActionManager::registerAction(QAction *action, const QString &id)
    \brief Register \a action in ActionManager.

    Registering of action allows you to use ActionManager::connect methods to connect to actions by
    ids, not direct pointers. You must provied unique id for each action you want to use.

    Also, you can retrieve action using ActionManager::action method to specify some properties.

    \sa ActionManager::registerMenu, ActionManager::unregister
*/
void ActionManager::registerAction(QAction *action, const QString &id)
{
    Q_D(ActionManager);

    if (!d->mapToAction.contains(id)) {
        d->mapToAction.insert(id, action);
        action->setProperty("id", id);
        QObject::connect(action, SIGNAL(triggered(bool)), d, SLOT(onTrigger(bool)));
    } else {
        qWarning() << "ActionManager::registerAction: Action with id" << id << "is already registered";
    }
}

/*!
    \fn void ActionManager::registerMenu(QMenu *menu, const QString &id)
    \brief Register \a menu in ActionManager.

    Allows retrieving menu using ActionManager::menu method. Also registers QMenu::menuAction with same
    \a id in manager. Returns menuAction.

    \sa ActionManager::registerAction, ActionManager::unregister
*/
void ActionManager::registerMenu(QMenu *menu, const QString &id)
{
    Q_D(ActionManager);

    d->mapToMenu.insert(id, menu);
    registerAction(menu->menuAction(), id);
}

/*!
    \fn void ActionManager::unregister(const QString &id)
    \brief Unregister action or menu with \a id.

    Removes all internal data of action or menu with \id from ActionManager. All connections are also
    disconnected.

    \sa ActionManager::registerAction, ActionManager::registerMenu
*/
void ActionManager::unregister(const QString &id)
{
    Q_D(ActionManager);

    d->mapToMenu.remove(id);
    QAction *action = d->mapToAction.take(id);
    if (action) {
        action->setProperty("id", QVariant());
        QObject::disconnect(action,  SIGNAL(triggered(bool)),
                            d, SLOT(onTrigger(bool)));
    }
}

/*!
    \fn bool ActionManager::connect(const QString &actionId, const QString &viewId, const char *slot)
    \brief Connects action with \a actionId to \a slot in \a viewId.

    This function connects QAction::triggered(bool) signal of action specified by \a actionId
    to slot in any view with id \a viewId. While changing perspectives, actions will be automatically
    enabled and disabled if view with specified id is present in current perspective, or not. In fact,
    there is no QObject::connect calls in this methods, so you don't warn about lot of connections -
    receiver is determined only on triggering action, which is rather fast.

    \sa ActionManager::disconnect
*/
bool ActionManager::connect(const QString &actionId, const QString &viewId, const char *slot)
{
    Q_D(ActionManager);

    if (!d->mapToAction.contains(actionId)) {
        qWarning() << "ActionManager::connect: No action" << actionId << "registered in ActionManager";
        return false;
    }

    d->connectionsViews.insertMulti(actionId, QPair<QString, const char*>(viewId, slot));
    d->mapToView.insertMulti(viewId, actionId);
    return true;
}

/*!
    \fn bool ActionManager::connect(const QString &actionId, QObject *receiver, const char *slot)
    \brief Connects action with \a actionId directly to \a slot in \a receiver.

    This function connects QAction::triggered(bool) signal of action specified by \a actionId
    to slot in \a receiver. Action is always enabled and you should manually enable/disable it when
    needed. You can also manually connect ot action using QObject::connect method to emulate behavior
    of this function.

    \sa ActionManager::action, ActionManager::disconnect
*/
bool ActionManager::connect(const QString &actionId, QObject *receiver, const char *slot)
{
    Q_D(ActionManager);

    if (!d->mapToAction.contains(actionId)) {
        qWarning() << "ActionManager::connect: No action" << actionId << "registered in ActionManager";
        return false;
    }

    if (!receiver) {
        qWarning() << "ActionManager::connect: null receiver";
        return false;
    }

    d->connectionsObjects.insertMulti(actionId, QPair<QObject*, const char*>(receiver, slot));
    return true;
}

/*!
    \fn bool ActionManager::connect(const QString &actionId, const char *slot)
    \brief Connects action with \a actionId to any widget that have method named \a slot.

    This function connects QAction::triggered(bool) signal of action specified by \a actionId
    to slot in widget. Actions enabled and disabled on changing focus if current focus widget have
    specified slot or not.

    \sa ActionManager::disconnect
*/
bool ActionManager::connect(const QString &actionId, const char *slot)
{
    Q_D(ActionManager);

    if (!d->mapToAction.contains(actionId)) {
        qWarning() << "ActionManager::connect: No action" << actionId << "registered in ActionManager";
        return false;
    }

    d->connectionsToWidgets.insertMulti(actionId, slot);
    return true;
}

/*!
    \fn void ActionManager::disconnect(const QString &actionId, const QString &viewId, const char *slot)
    \brief Disconnects previously established connection.

    Disconnects connection between action with id \a actionId and view with id \a viewId.

    \sa ActionManager::connect
*/
void ActionManager::disconnect(const QString &actionId, const QString &viewId, const char *slot)
{
    Q_D(ActionManager);

    d->connectionsViews.remove(actionId, QPair<QString, const char*>(viewId, slot));
}

/*!
    \fn void ActionManager::disconnect(const QString &actionId, QObject *receiver, const char *slot)
    \brief Disconnects previously established connection.

    Disconnects connection between action with id \a actionId and \a receiver.

    \sa ActionManager::connect
*/
void ActionManager::disconnect(const QString &actionId, QObject *receiver, const char *slot)
{
    Q_D(ActionManager);

    d->connectionsObjects.remove(actionId, QPair<QObject*, const char*>(receiver, slot));
}

/*!
    \fn void ActionManager::disconnect(const QString &actionId, const char *slot)
    \brief Disconnects previously established connection.

    Removes connection between action with id \a actionId and any widgets.

    \sa ActionManager::connect
*/
void ActionManager::disconnect(const QString &actionId, const char *slot)
{
    Q_D(ActionManager);

    d->connectionsToWidgets.remove(actionId, slot);
}
