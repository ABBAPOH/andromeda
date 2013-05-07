#include "commandcontainer.h"
#include "abstractcommand_p.h"

#include "actionmanager.h"
#include "command.h"

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

namespace GuiSystem {

class CommandContainerPrivate : public AbstractCommandPrivate
{
    Q_DECLARE_PUBLIC(CommandContainer)
public:
    explicit CommandContainerPrivate(CommandContainer *qq) :
        AbstractCommandPrivate(qq),
        menu(0)
    {}

    void addAbstractCommand(AbstractCommand *cmd, const QByteArray &weight);
    void setText(const QString &text);

public:
    QList<AbstractCommand *> commands;
    QList<QByteArray> weights;
    mutable QMenu *menu;
};

} // namespace GuiSystem

using namespace GuiSystem;

void CommandContainerPrivate::addAbstractCommand(AbstractCommand *cmd, const QByteArray &weight)
{
    Q_Q(CommandContainer);

    QByteArray w = weight;
    if (w.isEmpty())
        w = QString("%1").arg(commands.count(), 2, 10, QLatin1Char('0')).toLatin1();

    QList<QByteArray>::iterator i = qLowerBound(weights.begin(), weights.end(), w);
    int index = i - weights.begin();
    commands.insert(index, cmd);
    weights.insert(index, w);
    QObject::connect(cmd, SIGNAL(destroyed(QObject*)), q, SLOT(onDestroy(QObject*)));
}

void CommandContainerPrivate::setText(const QString &text)
{
    Q_Q(CommandContainer);

    if (this->text == text)
        return;

    this->text = text;
    if (menu)
        menu->setTitle(text);

    emit q->changed();
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
    AbstractCommand(*new CommandContainerPrivate(this), id, parent)
{
    ActionManager::instance()->registerContainer(this);
}

/*!
    \brief Destroys CommandContainer and unregister it in ActionManager.
*/
CommandContainer::~CommandContainer()
{
    ActionManager::instance()->unregisterContainer(this);
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
void CommandContainer::addCommand(AbstractCommand *c, const QByteArray &weight)
{
    if (!c)
        return;

    Q_D(CommandContainer);

    d->addAbstractCommand(c, weight);
}

void CommandContainer::addSeparator()
{
    addCommand(new Separator(this));
}

/*!
    \brief Destroys all gorups and remove added Commands.
*/
void CommandContainer::clear()
{
    Q_D(CommandContainer);

    d->commands.clear();
    d->weights.clear();
}

/*!
    \brief Returns list all Commands in this container.
*/
QList<Command *> CommandContainer::commands() const
{
    Q_D(const CommandContainer);

    QList<Command *> result;
    foreach (QObject *o, d->commands) {
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
    foreach (QObject *o, d->commands) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            result.append(cmd);
        }
    }
    return result;
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
    menu->setTitle(text());
    foreach (AbstractCommand *command, d->commands) {
        menu->addAction(command->createAction(menu));
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
    foreach (AbstractCommand *command, d->commands) {
        menuBar->addAction(command->createAction(menuBar));
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
    foreach (AbstractCommand *command, d->commands) {
        toolBar->addAction(command->createAction(toolBar));
    }
    return toolBar;
}

void CommandContainer::onDestroy(QObject *o)
{
    Q_D(CommandContainer);

    AbstractCommand *c = static_cast<AbstractCommand *>(o);
    int index = d->commands.indexOf(c);
    d->commands.removeAt(index);
    d->weights.removeAt(index);
}

QMenu * CommandContainer::createMenu(QWidget *parent) const
{
    Q_D(const CommandContainer);

    if (!d->menu)
        d->menu = new QMenu;

    return d->menu;
}

QToolBar * CommandContainer::createToolBar(QWidget *parent) const
{
    return new QToolBar(parent);
}

QAction * CommandContainer::createAction(QObject *parent) const
{
    return menu(qobject_cast<QWidget*>(parent))->menuAction();
}
