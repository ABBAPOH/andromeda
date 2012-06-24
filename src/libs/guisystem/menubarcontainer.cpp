#include "menubarcontainer.h"
#include "menubarcontainer_p.h"

#include <QtCore/QMetaEnum>

#include <QtGui/QAction>

#include "actionmanager.h"

#include <qglobal.h>

using namespace GuiSystem;

MenuBarContainerPrivate::MenuBarContainerPrivate(MenuBarContainer *qq) :
    q_ptr(qq)
{
    for (int i = 0; i < MenuBarContainer::StandardContainerCount; i++)
        containers[i] = 0;

    for (int i = 0; i < MenuBarContainer::StandardCommandCount; i++)
        commands[i] = 0;
}

void MenuBarContainerPrivate::createFileMenu()
{
    Q_Q(MenuBarContainer);

    containers[MenuBarContainer::FileMenu] = new CommandContainer(q->standardContainerName(MenuBarContainer::FileMenu), q);
    containers[MenuBarContainer::MenuBar]->addContainer(containers[MenuBarContainer::FileMenu]);

    // New window
    createCommand(MenuBarContainer::FileMenu,
                  MenuBarContainer::NewWindow,
                  QKeySequence::New,
                  Command::ApplicationCommand);

    // New tab
    createCommand(MenuBarContainer::FileMenu,
                  MenuBarContainer::NewTab,
                  QKeySequence("Ctrl+T"),
                  Command::WindowCommand);

    commands[MenuBarContainer::OpenFile] = new Command(q->standardCommandName(MenuBarContainer::OpenFile), q);
    commands[MenuBarContainer::OpenFile]->setDefaultShortcut(QKeySequence("Ctrl+O"));
    commands[MenuBarContainer::OpenFile]->setContext(Command::WindowCommand);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::OpenFile]);

    commands[MenuBarContainer::Close] = new Command(q->standardCommandName(MenuBarContainer::Close), q);
    commands[MenuBarContainer::Close]->setDefaultShortcut(QKeySequence("Ctrl+W"));
    commands[MenuBarContainer::Close]->setContext(Command::WindowCommand);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::Close]);

    containers[MenuBarContainer::FileMenu]->addCommand(new Separator(q));

    commands[MenuBarContainer::Save] = new Command(q->standardCommandName(MenuBarContainer::Save), q);
    commands[MenuBarContainer::Save]->setDefaultShortcut(QKeySequence("Ctrl+S"));
    commands[MenuBarContainer::Save]->setContext(Command::WindowCommand);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::Save]);

    commands[MenuBarContainer::SaveAs] = new Command(q->standardCommandName(MenuBarContainer::SaveAs), q);
    commands[MenuBarContainer::SaveAs]->setDefaultShortcut(QKeySequence("Ctrl+Shift+S"));
    commands[MenuBarContainer::SaveAs]->setContext(Command::WindowCommand);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::SaveAs]);

    containers[MenuBarContainer::FileMenu]->addCommand(new Separator(q));

    commands[MenuBarContainer::Refresh] = new Command(q->standardCommandName(MenuBarContainer::Refresh), q);
    commands[MenuBarContainer::Refresh]->setDefaultShortcut(QKeySequence("Ctrl+R"));
    commands[MenuBarContainer::Refresh]->setContext(Command::WindowCommand);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::Refresh]);

    commands[MenuBarContainer::Cancel] = new Command(q->standardCommandName(MenuBarContainer::Cancel), q);
    commands[MenuBarContainer::Cancel]->setContext(Command::WindowCommand);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::Cancel]);

    containers[MenuBarContainer::FileMenu]->addCommand(new Separator(q), "98");

    commands[MenuBarContainer::Quit] = new Command(q->standardCommandName(MenuBarContainer::Quit), q);
    commands[MenuBarContainer::Quit]->setDefaultShortcut(QKeySequence("Ctrl+Q"));
    commands[MenuBarContainer::Quit]->setContext(Command::ApplicationCommand);
    commands[MenuBarContainer::Quit]->setAttributes(Command::AttributeNonConfigurable);
    commands[MenuBarContainer::Quit]->commandAction()->setMenuRole(QAction::QuitRole);
    containers[MenuBarContainer::FileMenu]->addCommand(commands[MenuBarContainer::Quit], "99");
}

void MenuBarContainerPrivate::createEditMenu()
{
    Q_Q(MenuBarContainer);

    createMenu(MenuBarContainer::EditMenu);

    // Undo
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::Undo,
                  QKeySequence::Undo,
                  Command::WidgetCommand,
                  Command::AttributeUpdateText);

    // Redo
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::Redo,
                  QKeySequence::Redo,
                  Command::WidgetCommand,
                  Command::AttributeUpdateText);

    containers[MenuBarContainer::EditMenu]->addCommand(new Separator(q));

    // Cut
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::Cut,
                  QKeySequence::Cut,
                  Command::WidgetCommand,
                  Command::AttributeUpdateText);
    // Copy
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::Copy,
                  QKeySequence::Copy,
                  Command::WidgetCommand,
                  Command::AttributeUpdateText);

    // Paste
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::Paste,
                  QKeySequence::Paste);

    // Select all
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::SelectAll,
                  QKeySequence::SelectAll);

    containers[MenuBarContainer::EditMenu]->addCommand(new Separator(q));

    // Find
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::Find,
                  QKeySequence::Find);

    // Find next
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::FindNext,
                  QKeySequence::FindNext);

    // Find previous
    createCommand(MenuBarContainer::EditMenu,
                  MenuBarContainer::FindPrevious,
                  QKeySequence::FindPrevious);
}

void MenuBarContainerPrivate::createViewMenu()
{
    createMenu(MenuBarContainer::ViewMenu);
}

void MenuBarContainerPrivate::createToolsMenu()
{
    Q_Q(MenuBarContainer);

    createMenu(MenuBarContainer::ToolsMenu);

    Command *c = 0;

#ifndef Q_OS_MAC
    containers[MenuBarContainer::ToolsMenu]->addCommand(new Separator(q));
    createCommand(MenuBarContainer::ToolsMenu,
                  MenuBarContainer::ShowMenu,
                  QKeySequence("Alt+M"),
                  Command::WindowCommand);
#endif

    containers[MenuBarContainer::ToolsMenu]->addCommand(new Separator(q), "80");

    c = new Command(q->standardCommandName(MenuBarContainer::Preferences), q);
    c->setDefaultShortcut(QKeySequence::Preferences);
    c->setContext(Command::ApplicationCommand);
    c->setAttributes(Command::AttributeNonConfigurable);
    c->commandAction()->setMenuRole(QAction::PreferencesRole);
    commands[MenuBarContainer::Preferences] = c;
    containers[MenuBarContainer::ToolsMenu]->addCommand(c, "85");
}

void MenuBarContainerPrivate::createHelpMenu()
{
    createMenu(MenuBarContainer::HelpMenu);

    // About
    createCommand(MenuBarContainer::HelpMenu,
                  MenuBarContainer::About,
                  QKeySequence(),
                  Command::ApplicationCommand,
                  Command::AttributeNonConfigurable);
    commands[MenuBarContainer::About]->commandAction()->setMenuRole(QAction::AboutRole);

    // About Qt
    createCommand(MenuBarContainer::HelpMenu,
                  MenuBarContainer::AboutQt,
                  QKeySequence(),
                  Command::ApplicationCommand,
                  Command::AttributeNonConfigurable);
    commands[MenuBarContainer::AboutQt]->commandAction()->setMenuRole(QAction::AboutQtRole);
}

void MenuBarContainerPrivate::retranslateContainers()
{
    containers[MenuBarContainer::MenuBar]->setTitle(MenuBarContainer::tr("Menu bar"));

    containers[MenuBarContainer::FileMenu]->setTitle(MenuBarContainer::tr("File"));
    containers[MenuBarContainer::EditMenu]->setTitle(MenuBarContainer::tr("Edit"));
    containers[MenuBarContainer::ViewMenu]->setTitle(MenuBarContainer::tr("View"));
    containers[MenuBarContainer::ToolsMenu]->setTitle(MenuBarContainer::tr("Tools"));
    containers[MenuBarContainer::HelpMenu]->setTitle(MenuBarContainer::tr("Help"));
}

void MenuBarContainerPrivate::retranslateCommands()
{
    retranslateCommand(MenuBarContainer::NewWindow, MenuBarContainer::tr("New window"));
    retranslateCommand(MenuBarContainer::NewTab, MenuBarContainer::tr("New tab"));
    retranslateCommand(MenuBarContainer::OpenFile, MenuBarContainer::tr("Open..."));
    retranslateCommand(MenuBarContainer::Save, MenuBarContainer::tr("Save"));
    retranslateCommand(MenuBarContainer::SaveAs, MenuBarContainer::tr("Save As..."));

    retranslateCommand(MenuBarContainer::Close, MenuBarContainer::tr("Close"));
    retranslateCommand(MenuBarContainer::Refresh, MenuBarContainer::tr("Refresh"));
    retranslateCommand(MenuBarContainer::Cancel, MenuBarContainer::tr("Cancel"));

    retranslateCommand(MenuBarContainer::Quit, MenuBarContainer::tr("Quit"));

    retranslateCommand(MenuBarContainer::Undo, MenuBarContainer::tr("Undo"));
    retranslateCommand(MenuBarContainer::Redo, MenuBarContainer::tr("Redo"));

    retranslateCommand(MenuBarContainer::Cut, MenuBarContainer::tr("Cut"));
    retranslateCommand(MenuBarContainer::Copy, MenuBarContainer::tr("Copy"));
    retranslateCommand(MenuBarContainer::Paste, MenuBarContainer::tr("Paste"));
    retranslateCommand(MenuBarContainer::SelectAll, MenuBarContainer::tr("SelectAll"));

    retranslateCommand(MenuBarContainer::Find, MenuBarContainer::tr("Find"));
    retranslateCommand(MenuBarContainer::FindNext, MenuBarContainer::tr("Find next"));
    retranslateCommand(MenuBarContainer::FindPrevious, MenuBarContainer::tr("Find previous"));

#ifndef Q_OS_MAC
    retranslateCommand(MenuBarContainer::ShowMenu, MenuBarContainer::tr("Show menu"));
#endif
    retranslateCommand(MenuBarContainer::Preferences, MenuBarContainer::tr("Preferences"));

    retranslateCommand(MenuBarContainer::About, MenuBarContainer::tr("About..."));
    retranslateCommand(MenuBarContainer::AboutQt, MenuBarContainer::tr("About Qt..."));
}

void MenuBarContainerPrivate::createMenu(MenuBarContainer::StandardContainer menu)
{
    Q_Q(MenuBarContainer);

    CommandContainer * c = new CommandContainer(q->standardContainerName(menu), q);
    containers[menu] = c;
    containers[MenuBarContainer::MenuBar]->addContainer(c);
}

void MenuBarContainerPrivate::createCommand(MenuBarContainer::StandardContainer menu,
                                            MenuBarContainer::StandardCommand command,
                                            const QKeySequence &shortcut,
                                            Command::CommandContext context,
                                            Command::Attributes attributes)
{
    Q_Q(MenuBarContainer);

    Command *c = new Command(q->standardCommandName(command), q);
    c->setDefaultShortcut(shortcut);
    c->setContext(context);
    c->setAttributes(attributes);
    commands[command] = c;
    containers[menu]->addCommand(c);
}

void MenuBarContainerPrivate::retranslateCommand(MenuBarContainer::StandardCommand command, const QString &defaultText)
{
    commands[command]->setDefaultText(defaultText);
}

MenuBarContainer::MenuBarContainer(QObject *parent) :
    CommandContainer("MenuBar", parent),
    d_ptr(new MenuBarContainerPrivate(this))
{
}

MenuBarContainer::~MenuBarContainer()
{
    delete d_ptr;
}

MenuBarContainer *MenuBarContainer::instance()
{
    return qobject_cast<MenuBarContainer*>(ActionManager::instance()->container(standardContainerName(MenuBar)));
}

Command *MenuBarContainer::command(GuiSystem::MenuBarContainer::StandardCommand c)
{
    Q_D(MenuBarContainer);

    if (c < 0 || c >= StandardCommandCount)
        return 0;

    return d->commands[c];
}

CommandContainer *MenuBarContainer::container(MenuBarContainer::StandardContainer c)
{
    Q_D(MenuBarContainer);

    if (c < 0 || c >= StandardContainerCount)
        return 0;

    return d->containers[c];
}

const char *MenuBarContainer::standardCommandName(MenuBarContainer::StandardCommand standardAction)
{
    const QMetaObject &mo = MenuBarContainer::staticMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("StandardCommand"));
    return me.valueToKey(standardAction);
}

MenuBarContainer::StandardCommand MenuBarContainer::standardCommand(const char *name)
{
    const QMetaObject &mo = MenuBarContainer::staticMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("StandardCommand"));
    return StandardCommand(me.keysToValue(name));
}

const char *MenuBarContainer::standardContainerName(MenuBarContainer::StandardContainer standardContainer)
{
    const QMetaObject &mo = MenuBarContainer::staticMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("StandardContainer"));
    return me.valueToKey(standardContainer);
}

MenuBarContainer::StandardContainer MenuBarContainer::standardContainer(const char *name)
{
    const QMetaObject &mo = MenuBarContainer::staticMetaObject;
    QMetaEnum me = mo.enumerator(mo.indexOfEnumerator("StandardContainer"));
    return StandardContainer(me.keysToValue(name));
}

void MenuBarContainer::createMenus()
{
    Q_D(MenuBarContainer);

    d->containers[MenuBarContainer::MenuBar] = instance();

    d->createFileMenu();
    d->createEditMenu();
    d->createViewMenu();
    d->createToolsMenu();
    d->createHelpMenu();

    d->retranslateContainers();
    d->retranslateCommands();
}

void MenuBarContainer::retranslateUi()
{
    Q_D(MenuBarContainer);

    d->retranslateCommands();
    d->retranslateContainers();
}
