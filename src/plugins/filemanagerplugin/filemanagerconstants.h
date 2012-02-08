#ifndef FILEMANAGERCONSTANTS_H
#define FILEMANAGERCONSTANTS_H

namespace Constants {

namespace Menus {

const char * const SortBy = "Menu.SortBy";

} // namespace Menus

namespace MenuGroups {

const char * const FileInfo = "MenuGroups:3.File.Info";
const char * const FileChange = "MenuGroups:4.File.Change";

const char * const ViewViewMode = "MenuGroups:1.View.ViewMode";
const char * const ViewSortBy = "MenuGroups:2.View.SortBy";
const char * const ViewSortByOrder = "MenuGroups.View.ViewSortByOrder";

const char * const Locations = "MenuGroups:1.Goto.Locations";

} // namespace Groups

namespace Actions {

const char * const FileInfo = "Actions:3.FileInfo";
const char * const Open = "Actions:5.Open";
const char * const NewFolder = "Actions:1.NewFolder";
const char * const CopyFiles = "Actions:2.CopyFiles";
const char * const MoveFiles = "Actions:3.MoveFiles";
const char * const Rename = "Actions:4.Rename";
const char * const Remove = "Actions:5.Remove";

const char * const IconMode = "Actions.1.IconMode";
const char * const ColumnMode = "Actions.2.ColumnMode";
const char * const TreeMode = "Actions.3.TreeViewMode";
const char * const CoverFlowMode = "Actions.4.CoverFlowMode";
const char * const DualPane = "Actions.5.DualPane";

const char * const ShowHiddenFiles = "Actions.ShowHiddenFiles";

const char * const SortByName = "Actions.1.SortByName";
const char * const SortBySize = "Actions.2.SortBySize";
const char * const SortByType = "Actions.3.SortByType";
const char * const SortByDate = "Actions.4.SortByDate";
const char * const SortByDescendingOrder = "Actions.4.SortByDescendingOrder";

const char * const Goto = "Actions.Goto %1";

} // namespace Actions

} // namespace Constants

#endif // FILEMANAGERCONSTANTS_H
