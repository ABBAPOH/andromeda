#ifndef MENUBARCONTAINER_H
#define MENUBARCONTAINER_H

#include "commandcontainer.h"

namespace GuiSystem {

class MenuBarContainerPrivate;
class GUISYSTEM_EXPORT MenuBarContainer : public CommandContainer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MenuBarContainer)

public:
    enum StandardCommand {
        NewWindow,
        NewTab,
        OpenFile,
        Save,
        SaveAs,
        Close,
        Refresh,
        Cancel,

        Quit,

        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        SelectAll,

        Find,
        FindNext,
        FindPrevious,

        ShowMenu,
        Preferences,

        About,
        AboutQt,

        StandardCommandCount
    };
    Q_ENUMS(StandardCommand)

    enum StandardContainer {
        MenuBar,

        FileMenu,
        EditMenu,
        ViewMenu,
        ToolsMenu,
        HelpMenu,

        StandardContainerCount
    };
    Q_ENUMS(StandardContainer)

    explicit MenuBarContainer(QObject *parent = 0);
    ~MenuBarContainer();

    static MenuBarContainer *instance();

    Command *command(StandardCommand command);
    CommandContainer *container(StandardContainer container);

    static const char * standardCommandName(StandardCommand standardCommand);
    static StandardCommand standardCommand(const char *name);

    static const char * standardContainerName(StandardContainer standardContainer);
    static StandardContainer standardContainer(const char *name);

    void createMenus();
    void retranslateUi();

protected:
    MenuBarContainerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // MENUBARCONTAINER_H
