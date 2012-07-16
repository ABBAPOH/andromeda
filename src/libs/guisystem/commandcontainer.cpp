#include "commandcontainer.h"

#include "actionmanager.h"
#include "command.h"

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

namespace GuiSystem {

class CommandContainerPrivate
{
public:
    explicit CommandContainerPrivate(CommandContainer *qq) : q(qq) {}

    void addObject(QObject *o, const QByteArray &weight);

public:
    QByteArray id;
    QString title;

    QObjectList objects;
    QList<QByteArray> weights;

private:
    CommandContainer *q;
};

} // namespace GuiSystem

using namespace GuiSystem;

void CommandContainerPrivate::addObject(QObject *o, const QByteArray &weight)
{
    QByteArray w = weight;
    if (w.isEmpty())
        w = QString("%1").arg(objects.count(), 2, 10, QLatin1Char('0')).toLatin1();

    QList<QByteArray>::iterator i = qLowerBound(weights.begin(), weights.end(), w);
    int index = i - weights.begin();
    objects.insert(index, o);
    weights.insert(index, w);
    QObject::connect(o, SIGNAL(destroyed(QObject*)), q, SLOT(onDestroy(QObject*)));
}

/*!
    \class GuiSystem::CommandContainer

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
    d_ptr(new CommandContainerPrivate(this))
{
    Q_D(CommandContainer);

    d->id = id;

    ActionManager::instance()->registerContainer(this);
}

/*!
    \brief Destroys CommandContainer and unregister it in ActionManager.
*/
CommandContainer::~CommandContainer()
{
    ActionManager::instance()->unregisterContainer(this);

    delete d_ptr;
}

bool commandLessThen(QObject *o1, QObject *o2)
{
    QByteArray weight1, weight2;
    weight1 = o1->property("CommandContainer::weight").toByteArray();
    weight2 = o2->property("CommandContainer::weight").toByteArray();

    return weight1 < weight2;
}

#include <QDebug>

/*!
    \brief Adds \a command to a \a group.
*/
void CommandContainer::addCommand(Command *c, const QByteArray &weight)
{
    if (!c)
        return;

    Q_D(CommandContainer);

    d->addObject(c, weight);
}

/*!
    \brief Adds \a container to a \a group.
*/
void CommandContainer::addContainer(CommandContainer *c, const QByteArray &weight)
{
    if (!c)
        return;

    Q_D(CommandContainer);

    d->addObject(c, weight);
}

/*!
    \brief Destroys all gorups and remove added Commands.
*/
void CommandContainer::clear()
{
    Q_D(CommandContainer);

    d->objects.clear();
    d->weights.clear();
}

/*!
    \brief Returns list all Commands in this container.
*/
QList<Command *> CommandContainer::commands() const
{
    Q_D(const CommandContainer);

    QList<Command *> result;
    foreach (QObject *o, d->objects) {
        Command *cmd = qobject_cast<Command *>(o);
        if (cmd)
            result.append(cmd);
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
    foreach (QObject *o, d->objects) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            result.append(cmd);
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
    foreach (QObject *o, d->objects) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            menu->addAction(cmd->commandAction());
        } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
            menu->addMenu(container->menu());
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
    foreach (QObject *o, d->objects) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            menuBar->addAction(cmd->commandAction());
        } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
            menuBar->addMenu(container->menu());
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
    foreach (QObject *o, d->objects) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            toolBar->addAction(cmd->commandAction());
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

    int index = d->objects.indexOf(o);
    d->objects.removeAt(index);
    d->weights.removeAt(index);
}

QMenu * CommandContainer::createMenu(QWidget *parent) const
{
    return new QMenu(parent);
}

QToolBar * CommandContainer::createToolBar(QWidget *parent) const
{
    return new QToolBar(parent);
}
