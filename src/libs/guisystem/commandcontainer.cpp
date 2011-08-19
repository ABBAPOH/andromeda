#include "commandcontainer.h"

#include "actionmanager.h"
#include "command.h"

#include <QtCore/QDebug>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

namespace GuiSystem {

struct Group
{
    explicit Group(const QByteArray &id) : id(id), actionGroup(0) {}
    ~Group()
    {
        delete actionGroup;
    }

    QByteArray id;
    QObjectList objects;
    QActionGroup *actionGroup;
};

class CommandContainerPrivate
{
public:
    QByteArray id;
    QString title;
    QList<Group*> groups;

    Group* findGroup(const QByteArray &id);
};

} // namespace GuiSystem

using namespace GuiSystem;

Group * CommandContainerPrivate::findGroup(const QByteArray &id)
{
    foreach (Group *g, groups) {
        if (g->id == id)
            return g;
    }
    return 0;
}

CommandContainer::CommandContainer(const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandContainerPrivate)
{
    Q_D(CommandContainer);

    d->id = id;
    addGroup(QByteArray()); // default group

    ActionManager::instance()->registerContainer(this);
}

CommandContainer::~CommandContainer()
{
    ActionManager::instance()->unregisterContainer(this);

    qDeleteAll(d_func()->groups);
    delete d_ptr;
}

void CommandContainer::addCommand(Command *command, const QByteArray &group)
{
    Q_D(CommandContainer);

    Group *g = d->findGroup(group);
    if (!g) {
        qWarning() << "CommandContainer::addCommand" << "id =" << d->id << ": Can't find group " << group;
        return;
    }
    g->objects.append(command);
    g->actionGroup->addAction(command->commandAction());
}

void CommandContainer::addContainer(CommandContainer *container, const QByteArray &group)
{
    Q_D(CommandContainer);

    Group *g = d->findGroup(group);
    if (!g) {
        qWarning() << "CommandContainer::addCommand" << "id =" << d->id << ": Can't find group " << group;
        return;
    }
    g->objects.append(container);
}

void CommandContainer::addGroup(const QByteArray &id)
{
    Q_D(CommandContainer);

    Group *g = new Group(id);
    g->actionGroup = new QActionGroup(this);
    d->groups.append(g);
}

void CommandContainer::clear()
{
    Q_D(CommandContainer);

    qDeleteAll(d->groups);
    d->groups.clear();
}

QByteArray CommandContainer::id() const
{
    return d_func()->id;
}

QMenu * CommandContainer::menu() const
{
    Q_D(const CommandContainer);

    QMenu *menu = new QMenu;
    menu->setTitle(title());
    for (int i = 0; i < d->groups.size(); i++) {
        if (i != 0) {
            menu->addSeparator();
        }
        Group * g = d->groups[i];
        foreach (QObject *o, g->objects) {
            if (Command *cmd = qobject_cast<Command *>(o)) {
                menu->addAction(cmd->commandAction());
            } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
                menu->addMenu(container->menu());
            }
        }
    }

    return menu;
}

QMenuBar * CommandContainer::menuBar() const
{
    Q_D(const CommandContainer);

    QMenuBar *menuBar = new QMenuBar;
    for (int i = 0; i < d->groups.size(); i++) {
        Group * g = d->groups[i];
        foreach (QObject *o, g->objects) {
            if (Command *cmd = qobject_cast<Command *>(o)) {
                menuBar->addAction(cmd->commandAction());
            } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
                menuBar->addMenu(container->menu());
            }
        }
    }
    return menuBar;
}

QToolBar * CommandContainer::toolBar() const
{
    Q_D(const CommandContainer);

    QToolBar *toolBar = new QToolBar;
    for (int i = 0; i < d->groups.size(); i++) {
        Group * g = d->groups[i];
        foreach (QObject *o, g->objects) {
            if (Command *cmd = qobject_cast<Command *>(o)) {
                toolBar->addAction(cmd->commandAction());
            }
        }
    }
    return toolBar;
}

QString CommandContainer::title() const
{
    return d_func()->title;
}

void CommandContainer::setTitle(const QString &title)
{
    d_func()->title = title;
}
