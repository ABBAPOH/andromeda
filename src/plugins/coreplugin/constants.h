#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {

namespace Ids {

namespace Menus {

const char * const MenuBar = "Menus.MenuBar";

const char * const File = "Menus.File";
const char * const Edit = "Menus.Edit";
const char * const View = "Menus.View";
const char * const GoTo = "Menus.GoTo";
const char * const Tools = "Menus.Tools";

} // namespace Menus

namespace MenuGroups {

const char * const FileNew = "FileNew";
const char * const FileChange = "FileChange";
const char * const FileQuit = "FileQuit";

const char * const EditRedo = "EditRedo";
const char * const EditCopyPaste = "EditCopyPate";

const char * const ViewViewMode = "ViewViewMode";

const char * const GotoLocations = "GotoLocations";

} // namespace Groups

namespace Actions {
    
const char * const NewWindow = "Actions.NewWindow";
const char * const NewTab = "Actions.NewTab";
const char * const NewFolder = "Actions.NewFolder";
const char * const Open = "Actions.Open";
const char * const CloseTab = "Actions.CloseTab";
const char * const Rename = "Actions.Rename";
const char * const Remove = "Actions.Remove";
const char * const Exit = "Actions.Exit";

const char * const Undo = "Actions.Undo";
const char * const Redo = "Actions.Redo";
const char * const Cut = "Actions.Cut";
const char * const Copy = "Actions.Copy";
const char * const Paste = "Actions.Paste";
const char * const SelectAll = "Actions.SelectAll";
const char * const Plugins = "Actions.Plugins";

const char * const IconMode = "Actions.IconMode";
const char * const ColumnMode = "Actions.ColumnMode";
const char * const TreeMode = "Actions.TreeViewMode";
const char * const DualPane = "Actions.DualPane";

const char * const Back = "Actions.Back";
const char * const Forward = "Actions.Forward";
const char * const Up = "Actions.Up";

const char * const Home = "Actions.Home";

} // namespace Actions

} // namespace Ids

} // namespace Constants

#endif // CONSTANTS_H
