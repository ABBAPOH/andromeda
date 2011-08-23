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

    Group* findGroup(const QByteArray &id) const;
};

} // namespace GuiSystem

using namespace GuiSystem;

Group * CommandContainerPrivate::findGroup(const QByteArray &id) const
{
    foreach (Group *g, groups) {
        if (g->id == id)
            return g;
    }
    return 0;
}

/*!
    \class CommandContainer

    \brief The CommandContainer is an abstraction over menus and toolbars which allows
    to store Commands.

    This class is very similar to QMenu, QMenuBar and QToolBar, except it stores Commands,
    not QActions. However, each container can be represented using one of these classes.
*/

/*!
    \brief Constructs CommandContainer with \a id and register it in ActionManager.
*/
CommandContainer::CommandContainer(const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandContainerPrivate)
{
    Q_D(CommandContainer);

    d->id = id;
    addGroup(QByteArray()); // default group

    ActionManager::instance()->registerContainer(this);
}

/*!
    \brief Destroys CommandContainer and unregister it in ActionManager.
*/
CommandContainer::~CommandContainer()
{
    ActionManager::instance()->unregisterContainer(this);

    qDeleteAll(d_func()->groups);
    delete d_ptr;
}

/*!
    \brief Adds \a command to a \a group.
*/
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

/*!
    \brief Adds \a container to a \a group.
*/
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

/*!
    \brief Adds group with \a id to this container.

    Commands in group are combined together as a single unit; they are seprarated in menus and toolbars.
    If \a exclusive is set to true, only one Command in a group can be checked at a time.
*/
void CommandContainer::addGroup(const QByteArray &id, bool exclusive)
{
    Q_D(CommandContainer);

    Group *g = new Group(id);
    g->actionGroup = new QActionGroup(this);
    g->actionGroup->setExclusive(exclusive);
    d->groups.append(g);
}

/*!
    \brief Destroys all gorups and remove added Commands.
*/
void CommandContainer::clear()
{
    Q_D(CommandContainer);

    qDeleteAll(d->groups);
    d->groups.clear();
}

/*!
    \brief Returns list of Commands in group specified by \a id.
*/
QList<Command *> CommandContainer::commands(const QByteArray &id) const
{
    Q_D(const CommandContainer);

    QList<Command *> result;
    Group *g = d->findGroup(id);
    if (g) {
        foreach (QObject *o, g->objects) {
            if (Command *cmd = qobject_cast<Command *>(o)) {
                result.append(cmd);
            }
        }
    }
    return result;
}

/*!
    \property CommandContainer::id

    \brief CommandContainer's id, which is used to identify containers.
*/
QByteArray CommandContainer::id() const
{
    return d_func()->id;
}

/*!
    \brief Constructs QMenu that represents this CommandContainer.

    QMenu will contain all groups within this container; each group is separated.
    Triggering action in this menu will cause triggering action linked to Command.

    You are responsible for deleting menu.
*/
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

/*!
    \brief Constructs QMenuBar that represents this CommandContainer.

    QMenuBar will contain all groups within this container.
    Note, that no actions added to menu bar, only sub menus.

    You are responsible for deleting menu bar.
*/
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

/*!
    \brief Constructs QToolBar that represents this CommandContainer.

    QToolBar will contain all groups within this container; each group is separated.
    Note, that only actions added to tool bar.

    You are responsible for deleting tool bar.
*/
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

/*!
    \property CommandContainer::title()

    \brief CommandContainer's title, which is displayed as a menu's title.
*/
QString CommandContainer::title() const
{
    return d_func()->title;
}

void CommandContainer::setTitle(const QString &title)
{
    d_func()->title = title;
}
