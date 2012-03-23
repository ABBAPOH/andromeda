#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {

namespace Objects {

const char * const AlternateToolbar = "AlternateToolbar";

} // namespace Objects

namespace Menus {

const char * const MenuBar = "MenuBar";

const char * const File = "FileMenu";
const char * const Edit = "EditMenu";
const char * const View = "ViewMenu";
const char * const GoTo = "GoToMenu";
const char * const Windows = "Windows";
const char * const Tools = "ToolsMenu";
const char * const Help = "HelpMenu";
const char * const Dock = "DockMenu";

} // namespace Menus

namespace Actions {

const char * const NewWindow = "NewWindow";
const char * const NewTab = "NewTab";
const char * const CloseTab = "CloseTab";
const char * const Quit = "Quit";

const char * const Save = "Save";
const char * const SaveAs = "SaveAs";

const char * const Undo = "Undo";
const char * const Redo = "Redo";
const char * const Cut = "Cut";
const char * const Copy = "Copy";
const char * const Paste = "Paste";
const char * const SelectAll = "SelectAll";

const char * const Find = "Find";
const char * const FindNext = "FindNext";
const char * const FindPrevious = "FindPrevious";

const char * const ShowLeftPanel = "ShowLeftPanel";

const char * const Back = "Back";
const char * const Forward = "Forward";
const char * const Up = "Up";

const char * const About = "About";
const char * const AboutQt = "AboutQt";
const char * const Plugins = "Plugins";
const char * const Settings = "Settings";

const char * const Preferences = "Preferences";

} // namespace Actions

namespace Perspectives {

const char * const FileManagerPerspective = "FileManagerPerspective";

}

} // namespace Constants

#endif // CONSTANTS_H
