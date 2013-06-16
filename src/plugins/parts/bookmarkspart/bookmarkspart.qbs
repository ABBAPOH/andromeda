import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "BookmarksPart"

    Depends { name: "Qt"; submodules: ["core", "widgets"] }
    Depends { name: "IO" }
    Depends { name: "Widgets" }
    cpp.defines: [ "BOOKMARKS_LIBRARY" ]

    files : [
        "bookmark.cpp",
        "bookmark.h",
        "bookmarkdialog.cpp",
        "bookmarkdialog.h",
        "bookmarks_global.h",
        "bookmarksconstants.h",
        "bookmarksdocument.cpp",
        "bookmarksdocument.h",
        "bookmarkseditor.cpp",
        "bookmarkseditor.h",
        "bookmarksmenu.cpp",
        "bookmarksmenu.h",
        "bookmarksmodel.cpp",
        "bookmarksmodel.h",
        "bookmarksmodel_p.h",
        "bookmarksplugin.cpp",
        "bookmarksplugin.h",
        "bookmarksplugin_p.h",
        "bookmarksplugin.qrc",
        "bookmarkstoolbar.cpp",
        "bookmarkstoolbar.h",
        "bookmarkstoolmodel.cpp",
        "bookmarkstoolmodel.h",
        "bookmarkstoolwidget.cpp",
        "bookmarkstoolwidget.h",
        "bookmarkstoolwidget_p.h",
        "bookmarkswidget.cpp",
        "bookmarkswidget.h",
        "bookmarkswidget_p.h"
    ]
}
