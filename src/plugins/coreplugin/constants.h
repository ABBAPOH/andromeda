#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {

namespace Objects {

const char * const AlternateToolbar = "AlternateToolbar";

} // namespace Objects

namespace Menus {

const char * const MenuBar = "Menus.MenuBar";

const char * const File = "Menus:1.File";
const char * const Edit = "Menus:2.Edit";
const char * const View = "Menus:3.View";
const char * const GoTo = "Menus:4.GoTo";
const char * const Tools = "Menus:5.Tools";
const char * const Help = "Menus:6.Help";

} // namespace Menus

namespace MenuGroups {

const char * const FileNew = "MenuGroups:1.File.New";
const char * const FileSave = "MenuGroups:2.File.Save";
const char * const FileQuit = "MenuGroups:4.File.Quit";

const char * const EditRedo = "MenuGroups:1.Edit.Redo";
const char * const EditCopyPaste = "MenuGroups:2.Edit.CopyPate";
const char * const EditFind = "MenuGroups:3.Edit.Find";

const char * const ToolsPreferences = "MenuGroups:1.Tools.Preferences";

} // namespace Groups

namespace Actions {

const char * const NewWindow = "Actions:1.NewWindow";
const char * const NewTab = "Actions:2.NewTab";
const char * const CloseTab = "Actions:6.CloseTab";
const char * const Exit = "Actions.Exit";

const char * const Save = "Actions.Save";
const char * const SaveAs = "Actions.SaveAs";

const char * const Undo = "Actions.Undo";
const char * const Redo = "Actions.Redo";
const char * const Cut = "Actions.Cut";
const char * const Copy = "Actions.Copy";
const char * const Paste = "Actions.Paste";
const char * const SelectAll = "Actions.SelectAll";

const char * const Find = "Actions.Find";
const char * const FindNext = "Actions.FindNext";
const char * const FindPrevious = "Actions.FindPrevious";

const char * const ShowLeftPanel = "Actions.ShowLeftPanel";

const char * const Back = "Actions.Back";
const char * const Forward = "Actions.Forward";
const char * const Up = "Actions.Up";

const char * const About = "Actions.About";
const char * const AboutQt = "Actions.AboutQt";
const char * const Preferences = "Actions.Preferences";
const char * const Plugins = "Actions.Plugins";
const char * const Settings = "Actions.Settings";

} // namespace Actions

namespace Perspectives {

const char * const FileManagerPerspective = "FileManagerPerspective";

}

} // namespace Constants

#endif // CONSTANTS_H
