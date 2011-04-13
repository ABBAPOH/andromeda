#include "actionmanager.h"
#include "actionmanager_p.h"

#include "iview.h"
#include "perspectiveinstance.h"
#include "mainwindow.h"
#include "state.h"

using namespace GuiSystem;

void ActionManagerPrivate::onTrigger(bool checked)
{
    qDebug() << "ActionManagerPrivate::onActionTriggered" << checked;

    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);

    // TODO: use QObject instead of QAction here
    QString id = action->property("id").toString();

    foreach (ConnectionByObject pair, connectionsObjects.values(id)) {
        QObject *object = pair.first;
        const char *slot = pair.second;
        QMetaObject::invokeMethod(object, slot);
    }

    MainWindow *window = qobject_cast<MainWindow *>(qApp->activeWindow());
    if (!window) {
        return;
    }

    foreach (ConnectionById pair, connectionsViews.values(id)) {
        QString viewId = pair.first;
        const char *slot = pair.second;
        IView * view = window->currentState()->currentInstance()->view(viewId);
        if (!view) {
            qWarning() << "Cant find view with id" << viewId;
            continue;
        }
        QMetaObject::invokeMethod(view, slot);
    }
}

ActionManager::ActionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ActionManagerPrivate)
{
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

void ActionManager::registerMenu(QMenu *menu, const QString &id)
{
    Q_D(ActionManager);

    d->mapToMenu.insert(id, menu);
    registerAction(menu->menuAction(), id);
}

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

void ActionManager::disconnect(const QString &actionId, const QString &viewId, const char *slot)
{
    Q_D(ActionManager);

    d->connectionsViews.remove(actionId, QPair<QString, const char*>(viewId, slot));
}

void ActionManager::disconnect(const QString &actionId, QObject *receiver, const char *slot)
{
    Q_D(ActionManager);

    d->connectionsObjects.remove(actionId, QPair<QObject*, const char*>(receiver, slot));
}

