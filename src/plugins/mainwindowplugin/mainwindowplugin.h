#ifndef MAINWINDOWPLUGIN_H
#define MAINWINDOWPLUGIN_H

#include <iplugin.h>

namespace MainWindowPlugin {

const char * const MENU_BAR = "MENU_BAR";

const char * const MENU_FILE = "MENU_FILE";
const char * const MENU_EDIT = "MENU_EDIT";
const char * const MENU_VIEW = "MENU_VIEW";
const char * const MENU_GOTO = "MENU_GOTO";

const char * const ACTION_NEW_WINDOW = "ACTION_NEW_WINDOW";
const char * const ACTION_NEW_TAB = "ACTION_NEW_TAB";
const char * const ACTION_NEW_FOLDER = "ACTION_NEW_FOLDER";
const char * const ACTION_OPEN = "ACTION_OPEN";
const char * const ACTION_CLOSE_TAB = "ACTION_CLOSE_TAB";
const char * const ACTION_UNDO = "ACTION_UNDO";
const char * const ACTION_REDO = "ACTION_REDO";
const char * const ACTION_CUT = "ACTION_CUT";
const char * const ACTION_COPY = "ACTION_COPY";
const char * const ACTION_PASTE = "ACTION_PASTE";
const char * const ACTION_SELECT_ALL = "ACTION_SELECT_ALL";
const char * const ACTION_BACK = "ACTION_BACK";
const char * const ACTION_FORWARD = "ACTION_FORWARD";
const char * const ACTION_UP = "ACTION_UP";

class MainWindow;

class MainWindowPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    explicit MainWindowPluginImpl();

    virtual bool initialize();
    virtual void shutdown();

signals:

public slots:
    void newWindow();

private:
    void createActions();
};

}

#endif // MAINWINDOWPLUGIN_H
