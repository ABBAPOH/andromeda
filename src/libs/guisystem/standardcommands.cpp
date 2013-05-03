#include "standardcommands.h"
#include "standardcommands_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaEnum>
#include <QtCore/QtAlgorithms>

#include "actionmanager.h"
#include "command.h"

namespace GuiSystem {
namespace StandardCommands {

struct StandardCommandInfo
{
  StandardCommand id;
  const char *textId;
  QKeySequence::StandardKey shortcut;
  const char *text;
  const char *toolTip;
  const char *iconName;
};

static const StandardCommandInfo standardCommandInfo[] =
{
    // File Menu
    { New,       "New",       QKeySequence::New,        QT_TR_NOOP("&New"), QT_TR_NOOP("Create new document"), "document-new" },
    { NewTab,    "NewTab",    QKeySequence::UnknownKey, QT_TR_NOOP("New &tab"), QT_TR_NOOP("Create new tab"), "" },
    { NewWindow, "NewWindow", QKeySequence::New,        QT_TR_NOOP("New &window"), QT_TR_NOOP("Create new window"), "" },
    { Open,      "Open",      QKeySequence::Open,       QT_TR_NOOP("&Open"), QT_TR_NOOP("Open an existing document"), "document-open" },
    { Save,      "Save",      QKeySequence::Save,       QT_TR_NOOP("&Save"), QT_TR_NOOP("Save document"), "document-save" },
    { SaveAs,    "SaveAs",    QKeySequence::SaveAs,     QT_TR_NOOP("Save &As..."), QT_TR_NOOP("Save document under a new name"), "document-save-as" },
    { Close,     "Close",     QKeySequence::Close,      QT_TR_NOOP("&Close"), QT_TR_NOOP("Close document"), "window-close" },
    { Refresh,   "Refresh",   QKeySequence::Refresh,    QT_TR_NOOP("&Refresh"), QT_TR_NOOP("Reload current page"), "" },
    { Cancel,    "Cancel",    QKeySequence::UnknownKey, QT_TR_NOOP("&Cancel"), QT_TR_NOOP("Cancel current operation"), "" },
    { Quit,      "Quit",      QKeySequence::Quit,       QT_TR_NOOP("&Quit"), QT_TR_NOOP("Quit application"), "application-exit" },

    // Edit Menu
    { Undo,        "Undo",        QKeySequence::Undo,         QT_TR_NOOP("&Undo"), QT_TR_NOOP("Undo last action"), "edit-undo" },
    { Redo,        "Redo",        QKeySequence::Redo,         QT_TR_NOOP("Re&do"), QT_TR_NOOP("Redo last undone action"), "edit-redo" },
    { Cut,         "Cut",         QKeySequence::Cut,          QT_TR_NOOP("Cu&t"), QT_TR_NOOP("Cut selection to clipboard"), "edit-cut" },
    { Copy,        "Copy",        QKeySequence::Copy,         QT_TR_NOOP("&Copy"), QT_TR_NOOP("Copy selection to clipboard"), "edit-copy" },
    { Paste,       "Paste",       QKeySequence::Paste,        QT_TR_NOOP("&Paste"), QT_TR_NOOP("Paste clipboard content"), "edit-paste" },
    { Clear,       "Clear",       QKeySequence::UnknownKey,   QT_TR_NOOP("C&lear"), QT_TR_NOOP("Clear"), "edit-clear" },
    { SelectAll,   "SelectAll",   QKeySequence::SelectAll,    QT_TR_NOOP("Select &All"), QT_TR_NOOP("Select all"), "edit-select-all" },
    { Deselect,    "Deselect",    QKeySequence::UnknownKey,   QT_TR_NOOP("Dese&lect"), QT_TR_NOOP("Deselect"), "" },
    { Find,        "Find",        QKeySequence::Find,         QT_TR_NOOP("&Find"), QT_TR_NOOP("Find"), "edit-find" },
    { FindNext,    "FindNext",    QKeySequence::FindNext,     QT_TR_NOOP("Find &Next"), QT_TR_NOOP("Find Next"), "go-down-search" },
    { FindPrev,    "FindPrev",    QKeySequence::FindPrevious, QT_TR_NOOP("Find Pre&vious"), QT_TR_NOOP("Find Previous"), "go-up-search" },
    { Preferences, "Preferences", QKeySequence::Preferences,  QT_TR_NOOP("&Preferences"), QT_TR_NOOP("Show application preferences"), "configure" },

    // View Menu
    { ShowMenubar,   "ShowMenubar",   QKeySequence::UnknownKey, QT_TR_NOOP("Show &Menubar"), QT_TR_NOOP("Show or hide menubar"), "show-menu" },
    { ShowToolBar,   "ShowToolbar",   QKeySequence::UnknownKey, QT_TR_NOOP("Show &Toolbar"), QT_TR_NOOP("Show or hide toolbar"), "" },
    { ShowStatusBar, "ShowStatusbar", QKeySequence::UnknownKey, QT_TR_NOOP("Show St&atusbar"), QT_TR_NOOP("Show or hide statusbar"), "" },
    { ShowLeftPanel, "ShowLeftPanel", QKeySequence::UnknownKey, QT_TR_NOOP("Show &left panel"), QT_TR_NOOP("Show or hide left panel"), "" },
    { FullScreen,    "FullScreen",    QKeySequence::UnknownKey, QT_TR_NOOP("F&ull Screen Mode"), QT_TR_NOOP("Enter or exit fullscreen mode"), "view-fullscreen" },

    // Go Menu
    { Up,      "Up",      QKeySequence::UnknownKey, QT_TR_NOOP("&Up"), QT_TR_NOOP("Go up"), "go-up" },
    { Back,    "Back",    QKeySequence::Back,       QT_TR_NOOP("Back"), QT_TR_NOOP("Go back"), "go-previous" },
    { Forward, "Forward", QKeySequence::Forward,    QT_TR_NOOP("Forward"), QT_TR_NOOP("Go forward"), "go-next" },

    // Bookmarks Menu
    { AddBookmark,   "AddBookmark", QKeySequence::UnknownKey,   QT_TR_NOOP("&Add Bookmark"), QT_TR_NOOP("Add Bookmark"), "bookmark-new" },
    { EditBookmarks, "EditBookmarks", QKeySequence::UnknownKey, QT_TR_NOOP("&Edit Bookmarks..."), QT_TR_NOOP("Edit Bookmarks"), "bookmarks-organize" },

    // Help menu
    { Help,    "Help",    QKeySequence::UnknownKey, QT_TR_NOOP("&Help"), QT_TR_NOOP("Show help"), "help-contents" },
    { About,   "About",   QKeySequence::UnknownKey, QT_TR_NOOP("&About..."), QT_TR_NOOP("About"), "" },
    { AboutQt, "AboutQt", QKeySequence::UnknownKey, QT_TR_NOOP("About &Qt..."), QT_TR_NOOP("About Qt"), "" }
};

} // namespace StandardCommands
} // namespace GuiSystem

using namespace GuiSystem;

StandardCommandsPrivate::StandardCommandsPrivate()
{
    qFill(commands, &commands[StandardCommands::StandardCommandCount], (Command*)0);
}

Q_GLOBAL_STATIC(StandardCommandsPrivate, staticInstance)
StandardCommandsPrivate * StandardCommandsPrivate::instance()
{
    return staticInstance();
}

Command * StandardCommandsPrivate::createCommand(StandardCommands::StandardCommand command)
{
    QByteArray name = commandId(command);
    Command *c = new Command(name, ActionManager::instance());

    c->setDefaultShortcut(commandShortcut(command));
    c->setIcon(commandIcon(command));

    switch (command) {
    case StandardCommands::Quit:
        c->commandAction()->setMenuRole(QAction::QuitRole);
        break;
    case StandardCommands::Preferences:
        c->commandAction()->setMenuRole(QAction::PreferencesRole);
        break;
    case StandardCommands::About:
        c->commandAction()->setMenuRole(QAction::AboutRole);
        break;
    case StandardCommands::AboutQt:
        c->commandAction()->setMenuRole(QAction::AboutQtRole);
        break;
    default:
        break;
    }

    return c;
}

inline void StandardCommandsPrivate::retranslateCommand(Command *c, StandardCommands::StandardCommand command)
{
    c->setText(commandText(command));
    c->setToolTip(commandToolTip(command));
}

inline QByteArray StandardCommandsPrivate::commandId(StandardCommands::StandardCommand command)
{
    const StandardCommands::StandardCommandInfo &info = StandardCommands::standardCommandInfo[command];
    Q_ASSERT(info.id == command);
    return info.textId;
}

inline QString StandardCommandsPrivate::commandText(StandardCommands::StandardCommand command)
{
    const StandardCommands::StandardCommandInfo &info = StandardCommands::standardCommandInfo[command];
    Q_ASSERT(info.id == command);
    return qApp->translate("GuiSystem::StandardCommands", info.text);
}

inline QString StandardCommandsPrivate::commandToolTip(StandardCommands::StandardCommand command)
{
    const StandardCommands::StandardCommandInfo &info = StandardCommands::standardCommandInfo[command];
    Q_ASSERT(info.id == command);
    return qApp->translate("GuiSystem::StandardCommands", info.toolTip);
}

QKeySequence StandardCommandsPrivate::commandShortcut(StandardCommands::StandardCommand command)
{
    const StandardCommands::StandardCommandInfo &info = StandardCommands::standardCommandInfo[command];
    Q_ASSERT(info.id == command);
    if (info.shortcut == QKeySequence::UnknownKey) {
        switch (command) {
        case StandardCommands::NewTab:
            return QKeySequence("Ctrl+T");
//        case StandardCommands::Close:
//            return QKeySequence("Ctrl+W");
        case StandardCommands::Up:
#ifdef Q_OS_MAC
            return QKeySequence("Ctrl+Up");
#else
            return QKeySequence("Alt+Up");
#endif
        case StandardCommands::ShowLeftPanel:
            return QKeySequence("Ctrl+L");
        case StandardCommands::ShowStatusBar:
            return QKeySequence("Ctrl+B");
        default:
            break;
        }
    } else {
        return QKeySequence(info.shortcut);
    }
    return QKeySequence();
}

QIcon StandardCommandsPrivate::commandIcon(StandardCommands::StandardCommand command)
{
    const StandardCommands::StandardCommandInfo &info = StandardCommands::standardCommandInfo[command];
    Q_ASSERT(info.id == command);
    const QString iconName = QLatin1String(info.iconName);
    return QIcon::fromTheme(iconName,
                            QIcon(QString(":/guisystem/icons/%1").arg(iconName)));
}

Command * StandardCommands::standardCommand(StandardCommands::StandardCommand command)
{
    StandardCommandsPrivate *d = StandardCommandsPrivate::instance();
    if (command <= StandardCommands::NoCommand || command >= StandardCommands::StandardCommandCount)
        return 0;
    if (d->commands[command] == 0) {
        QMutexLocker l(&d->mutex);
        if (d->commands[command] == 0) {
            Command *c = StandardCommandsPrivate::createCommand(command);
            StandardCommandsPrivate::retranslateCommand(c, command);
            d->commands[command] = c;
        }
    }
    return d->commands[command];
}

void StandardCommands::retranslateCommands()
{
    StandardCommandsPrivate *d = StandardCommandsPrivate::instance();
    for (int cmd = 0; cmd < StandardCommandCount; cmd++) {
        Command *command = d->commands[StandardCommand(cmd)];
        if (command) {
            StandardCommandsPrivate::retranslateCommand(command, StandardCommand(cmd));
        }
    }
}
