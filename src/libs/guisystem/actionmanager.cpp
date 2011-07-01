#include "actionmanager.h"
#include "actionmanager_p.h"

#include "command.h"
#include "commandcontainer.h"

#include "iview.h"
#include "perspectiveinstance.h"
#include "perspectivewidget.h"

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

    PerspectiveWidget *activeWidget = PerspectiveWidget::activeWidget();
    foreach (const ConnectionById &pair, connectionsViews.values(id)) {
        QString viewId = pair.first;
        const char *slot = pair.second;
        IView * view = activeWidget->currentInstance()->view(viewId);
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

void ActionManagerPrivate::onFocusChange(QWidget */*old*/, QWidget *now)
{
    foreach (Command *c, activeCommands) {
        c->setRealAction(0);
    }
    activeCommands.clear();

    QWidget *w = now;
    while (w) {
        foreach (QAction *action, w->actions()) {
            QString id = action->objectName();
            if (!id.isEmpty()) {
                Command *c = qobject_cast<Command *>(objects.value(id));
                if (c) {
                    c->setRealAction(action);
                    activeCommands.append(c);
                }
            }
        }
        w = w->parentWidget();
    }
//    foreach (const QString &id, connectionsToWidgets.keys()) {
//        QAction *action = mapToAction.value(id);
//        if (!action)
//            continue;
//        const QMetaObject *metaObject = now ? now->metaObject() : 0;
//        if (metaObject && metaObject->indexOfSlot(connectionsToWidgets.value(id)) != -1)
//            action->setEnabled(true);
//        else
//            action->setEnabled(false);
//    }
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

void ActionManager::registerCommand(Command *cmd)
{
    Q_D(ActionManager);

    d->objects.insert(cmd->id(), cmd);
    if (!cmd->parent())
        cmd->setParent(this);
}

void ActionManager::registerContainer(CommandContainer *c)
{
    Q_D(ActionManager);

    d->objects.insert(c->id(), c);
    if (!c->parent())
        c->setParent(this);
}

Command * ActionManager::command(const QString &id)
{
    return qobject_cast<Command *>(d_func()->objects.value(id));
}

CommandContainer * ActionManager::container(const QString &id)
{
    return qobject_cast<CommandContainer *>(d_func()->objects.value(id));
}
