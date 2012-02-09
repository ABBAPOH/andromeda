#include "commandcontainer.h"

#include "actionmanager.h"
#include "command.h"

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

namespace GuiSystem {

struct Group
{
    explicit Group(const QByteArray &arr) : id(arr) {}
    ~Group() {}

    QByteArray id;
    QObjectList objects;
};

class CommandContainerPrivate
{
public:
    QByteArray id;
    QString title;
    QList<Group*> groups;

    Group *findGroup(const QByteArray &id) const;
    Group *addGroup(const QByteArray &id);
};

} // namespace GuiSystem

using namespace GuiSystem;

bool groupLessThen(Group *g1, Group *g2)
{
    return (g1->id < g2->id);
}

Group * CommandContainerPrivate::findGroup(const QByteArray &id) const
{
    foreach (Group *g, groups) {
        if (g->id == id)
            return g;
    }

    return 0;
}

Group * CommandContainerPrivate::addGroup(const QByteArray &id)
{
    foreach (Group *g, groups) {
        if (g->id == id)
            return g;
    }

    Group *g = new Group(id);
    QList<Group*>::iterator i = qLowerBound(groups.begin(), groups.end(), g, groupLessThen);
    groups.insert(i, g);
    return g;
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
    d->addGroup(QByteArray()); // default group

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

bool commandLessThen(QObject *o1, QObject *o2)
{
    QByteArray weight1, weight2;
    weight1 = o1->property("CommandContainer::weight").toByteArray();
    weight2 = o2->property("CommandContainer::weight").toByteArray();

    return weight1 < weight2;
}

/*!
    \brief Adds \a command to a \a group.
*/
void CommandContainer::addCommand(Command *c, const QByteArray &group, const QByteArray &weight)
{
    Q_D(CommandContainer);

    Group *g = d->addGroup(group);
    QByteArray w = weight;
    if (w.isEmpty())
        w = QString::number(g->objects.count()).toLatin1();
    c->setProperty("CommandContainer::weight", w);

    QObjectList::iterator i = qLowerBound(g->objects.begin(), g->objects.end(), c, commandLessThen);
    g->objects.insert(i, c);
    connect(c, SIGNAL(destroyed(QObject*)), SLOT(onDestroy(QObject*)));
}

/*!
    \brief Adds \a container to a \a group.
*/
void CommandContainer::addContainer(CommandContainer *c, const QByteArray &group, const QByteArray &weight)
{
    Q_D(CommandContainer);

    Group *g = d->addGroup(group);
    QByteArray w = weight;
    if (w.isEmpty())
        w = QString::number(g->objects.count()).toLatin1();
    c->setProperty("CommandContainer::weight", w);

    QObjectList::iterator i = qLowerBound(g->objects.begin(), g->objects.end(), c, commandLessThen);
    g->objects.insert(i, c);
    connect(c, SIGNAL(destroyed(QObject*)), SLOT(onDestroy(QObject*)));
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
    \brief Returns list all Commands in this container.
*/
QList<Command *> CommandContainer::commands() const
{
    Q_D(const CommandContainer);

    QList<Command *> result;
    foreach (Group *g, d->groups) {
        foreach (QObject *o, g->objects) {
            Command *cmd = qobject_cast<Command *>(o);
            if (cmd)
                result.append(cmd);
        }
    }
    return result;
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
QMenu * CommandContainer::menu(QWidget *parent) const
{
    Q_D(const CommandContainer);

    QMenu *menu = createMenu(parent);
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
QToolBar * CommandContainer::toolBar(QWidget *parent) const
{
    Q_D(const CommandContainer);

    QToolBar *toolBar = createToolBar(parent);
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

void CommandContainer::onDestroy(QObject *o)
{
    Q_D(CommandContainer);

    foreach (Group *g, d->groups) {
        g->objects.removeAll(o);
    }
}

QMenu * CommandContainer::createMenu(QWidget *parent) const
{
    return new QMenu(parent);
}

QToolBar * CommandContainer::createToolBar(QWidget *parent) const
{
    return new QToolBar(parent);
}
