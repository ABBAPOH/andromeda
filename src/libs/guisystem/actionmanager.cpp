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
};

} // namespace GuiSystem

using namespace GuiSystem;

ActionManager::ActionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ActionManagerPrivate)
{
    qApp->installEventFilter(this);
}

ActionManager::~ActionManager()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(ActionManager, get_instance)
ActionManager *ActionManager::instance()
{
    return get_instance();
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

void ActionManager::unregisterCommand(Command *cmd)
{
    Q_D(ActionManager);

    d->objects.remove(cmd->id());
    if (cmd->parent() == this)
        cmd->deleteLater();
}

void ActionManager::unregisterContainer(CommandContainer *c)
{
    Q_D(ActionManager);

    d->objects.remove(c->id());
    if (c->parent() == this)
        c->deleteLater();
}

bool ActionManager::eventFilter(QObject *o, QEvent *e)
{
    if (o->isWidgetType()) {
        QWidget *w = static_cast<QWidget*>(o);

        if (e->type() == QEvent::Show) {
            if (w->isActiveWindow()) {
                setActionsEnabled(w, true, Command::WindowCommand);
            }
        } else if (e->type() == QEvent::Hide) {
            if (w->isActiveWindow()) {
                setActionsEnabled(w, false, Command::WindowCommand);
            }
        } else if (e->type() == QEvent::FocusIn) {
            while (w) {
                setActionsEnabled(w, true, Command::WidgetCommand);
                w = w->parentWidget();
            }
        } else if (e->type() == QEvent::FocusOut) {
            while (w) {
                setActionsEnabled(w, false, Command::WidgetCommand);
                w = w->parentWidget();
            }
        } else if (e->type() == QEvent::ActivationChange) {
            bool enable = w->isActiveWindow();
            QWidgetList widgets = w->findChildren<QWidget*>();
            widgets.prepend(w);
            foreach (QWidget *w, widgets) {
                setActionsEnabled(w, enable, Command::WindowCommand);
            }
        }
    }
    return QObject::eventFilter(o, e);
}

void ActionManager::setActionsEnabled(QWidget *w, bool enabled, Command::CommandContext context)
{
    Q_D(ActionManager);

    foreach (QAction *action, w->actions()) {
        QString id = action->objectName();
        if (!id.isEmpty()) {
            Command *c = qobject_cast<Command *>(d->objects.value(id));
            if (c && c->context() == context) {
                c->setRealAction(enabled ? action : 0);
            }
        }
    }
}
