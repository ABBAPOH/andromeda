#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {

namespace Menus {

const char * const MenuBar = "Menus.MenuBar";

const char * const File = "Menus:1.File";
const char * const Edit = "Menus:2.Edit";
const char * const View = "Menus:3.View";
const char * const GoTo = "Menus:4.GoTo";
const char * const Tools = "Menus:5.Tools";
const char * const Help = "Menus:6.Help";
const char * const SortBy = "Menus.SortBy";

} // namespace Menus

namespace MenuGroups {

const char * const FileNew = "MenuGroups:1.File.New";
const char * const FileInfo = "MenuGroups:2.File.Info";
const char * const FileChange = "MenuGroups:3.File.Change";
const char * const FileQuit = "MenuGroups:4.File.Quit";

const char * const EditRedo = "MenuGroups:1.Edit.Redo";
const char * const EditCopyPaste = "MenuGroups:2.Edit.CopyPate";

const char * const ViewViewMode = "MenuGroups:1.View.ViewMode";
const char * const ViewSortBy = "MenuGroups:2.View.SortBy";
const char * const ViewSortByOrder = "MenuGroups.View.ViewSortByOrder";

const char * const Locations = "MenuGroups:1.Goto.Locations";

const char * const ToolsPreferences = "MenuGroups:1.Tools.Preferences";

} // namespace Groups

namespace Actions {

const char * const NewWindow = "Actions:1.NewWindow";
const char * const NewTab = "Actions:2.NewTab";
const char * const FileInfo = "Actions:3.FileInfo";
const char * const NewFolder = "Actions:4.NewFolder";
const char * const Open = "Actions:5.Open";
const char * const CloseTab = "Actions:6.CloseTab";
const char * const Rename = "Actions:7.Rename";
const char * const Remove = "Actions:8.Remove";
const char * const Exit = "Actions:9.Exit";

const char * const Undo = "Actions.Undo";
const char * const Redo = "Actions.Redo";
const char * const Cut = "Actions.Cut";
const char * const Copy = "Actions.Copy";
const char * const Paste = "Actions.Paste";
const char * const SelectAll = "Actions.SelectAll";

const char * const IconMode = "Actions.1.IconMode";
const char * const ColumnMode = "Actions.2.ColumnMode";
const char * const TreeMode = "Actions.3.TreeViewMode";
const char * const CoverFlowMode = "Actions.4.CoverFlowMode";
const char * const DualPane = "Actions.5.DualPane";

const char * const SortByName = "Actions.1.SortByName";
const char * const SortBySize = "Actions.2.SortBySize";
const char * const SortByType = "Actions.3.SortByType";
const char * const SortByDate = "Actions.4.SortByDate";
const char * const SortByDescendingOrder = "Actions.4.SortByDescendingOrder";

const char * const ShowLeftPanel = "Actions.ShowLeftPanel";
const char * const ShowHiddenFiles = "Actions.ShowHiddenFiles";

const char * const Back = "Actions.Back";
const char * const Forward = "Actions.Forward";
const char * const Up = "Actions.Up";

const char * const Goto = "Actions.Goto %1";

const char * const About = "Actions.About";
const char * const AboutQt = "Actions.AboutQt";
const char * const Preferences = "Actions.Preferences";
const char * const Plugins = "Actions.Plugins";

} // namespace Actions

namespace Perspectives {

const char * const FileManagerPerspective = "FileManagerPerspective";

}

} // namespace Constants

#endif // CONSTANTS_H
