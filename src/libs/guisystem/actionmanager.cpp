#include "actionmanager.h"

#include "command.h"
#include "commandcontainer.h"

#include <QtCore/QHash>
#include <QtGui/QApplication>
#include <QtGui/QAction>

namespace GuiSystem {

class ActionManagerPrivate
{
public:
    QHash<QString, QObject *> objects;
    QList<Command *> activeCommands;
};

} // namespace GuiSystem

using namespace GuiSystem;

ActionManager::ActionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ActionManagerPrivate)
{
    QObject::connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
                     SLOT(onFocusChange(QWidget*,QWidget*)));
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

void ActionManager::onFocusChange(QWidget */*old*/, QWidget *now)
{
    Q_D(ActionManager);

    foreach (Command *c, d->activeCommands) {
        c->setRealAction(0);
    }
    d->activeCommands.clear();

    QWidget *w = now;
    while (w) {
        foreach (QAction *action, w->actions()) {
            QString id = action->objectName();
            if (!id.isEmpty()) {
                Command *c = qobject_cast<Command *>(d->objects.value(id));
                if (c) {
                    c->setRealAction(action);
                    d->activeCommands.append(c);
                }
            }
        }
        w = w->parentWidget();
    }
}
