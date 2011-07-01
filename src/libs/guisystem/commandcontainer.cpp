#include "commandcontainer.h"

#include "command.h"

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

namespace GuiSystem {

class CommandContainerPrivate
{
public:
    QString id;
    QObjectList commands;
    QString title;
};

} // namespace GuiSystem

using namespace GuiSystem;

CommandContainer::CommandContainer(const QString &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandContainerPrivate)
{
    Q_D(CommandContainer);

    d->id = id;
}

CommandContainer::~CommandContainer()
{
    delete d_ptr;
}

void CommandContainer::addCommand(Command *command)
{
    d_func()->commands.append(command);
}

void CommandContainer::addContainer(CommandContainer *container)
{
    d_func()->commands.append(container);
}

void CommandContainer::clear()
{
    d_func()->commands.clear();
}

QString CommandContainer::id() const
{
    return d_func()->id;
}

QMenu * CommandContainer::menu() const
{
    Q_D(const CommandContainer);

    QMenu *menu = new QMenu;
    menu->setTitle(title());
    foreach (QObject *o, d->commands) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            menu->addAction(cmd->action());
        } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
            menu->addMenu(container->menu());
        }
    }
    return menu;
}

QMenuBar * CommandContainer::menuBar() const
{
    Q_D(const CommandContainer);

    QMenuBar *menuBar = new QMenuBar;
    foreach (QObject *o, d->commands) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            menuBar->addAction(cmd->action());
        } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
            menuBar->addMenu(container->menu());
        }
    }
    return menuBar;
}

QString CommandContainer::title() const
{
    return d_func()->title;
}

void CommandContainer::setTitle(const QString &title)
{
    d_func()->title = title;
}
