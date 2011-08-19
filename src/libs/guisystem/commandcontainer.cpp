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
    QByteArray id;
    QObjectList commands;
    QString title;
};

} // namespace GuiSystem

using namespace GuiSystem;

CommandContainer::CommandContainer(const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandContainerPrivate)
{
    Q_D(CommandContainer);

    d->id = id;

    ActionManager::instance()->registerContainer(this);
}

CommandContainer::~CommandContainer()
{
    ActionManager::instance()->unregisterContainer(this);

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

void CommandContainer::addSeparator()
{
    Command *cmd = new Command(QByteArray(), this);
    cmd->setSeparator(true);
    addCommand(cmd);
}

void CommandContainer::clear()
{
    d_func()->commands.clear();
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
    foreach (QObject *o, d->commands) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            menu->addAction(cmd->commandAction());
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
            menuBar->addAction(cmd->commandAction());
        } else if (CommandContainer *container = qobject_cast<CommandContainer *>(o)) {
            menuBar->addMenu(container->menu());
        }
    }
    return menuBar;
}

QToolBar * CommandContainer::toolBar() const
{
    Q_D(const CommandContainer);

    QToolBar *toolBar = new QToolBar;
    foreach (QObject *o, d->commands) {
        if (Command *cmd = qobject_cast<Command *>(o)) {
            toolBar->addAction(cmd->commandAction());
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
