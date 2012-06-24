#ifndef MENUBARCONTAINER_P_H
#define MENUBARCONTAINER_P_H

#include "menubarcontainer.h"

#include <QtGui/QKeySequence>

#include "command.h"

namespace GuiSystem {

class MenuBarContainerPrivate
{
    Q_DECLARE_PUBLIC(MenuBarContainer)

public:
    explicit MenuBarContainerPrivate(MenuBarContainer *qq);

    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createToolsMenu();
    void createHelpMenu();
    void retranslateContainers();
    void retranslateCommands();

    void createMenu(MenuBarContainer::StandardContainer menu);
    void createCommand(MenuBarContainer::StandardContainer menu, MenuBarContainer::StandardCommand command,
                       const QKeySequence &shortcut = QKeySequence(),
                       Command::CommandContext context = Command::WidgetCommand,
                       Command::Attributes attributes = 0);

    void retranslateCommand(MenuBarContainer::StandardCommand command, const QString &defaultText);

public:
    Command *commands[MenuBarContainer::StandardCommandCount];
    CommandContainer *containers[MenuBarContainer::StandardContainerCount];

private:
    MenuBarContainer *q_ptr;
};

} // namespace GuiSystem

#endif // MENUBARCONTAINER_P_H
