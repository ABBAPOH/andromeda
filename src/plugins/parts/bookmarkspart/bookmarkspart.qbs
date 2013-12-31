import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "BookmarksPart"

    Depends { name: "Qt"; submodules: ["core", "widgets"] }
    Depends { name: "Bookmarks" }
    Depends { name: "IO" }
    Depends { name: "Widgets" }

    files : [
        "bookmarksconstants.h",
        "bookmarksdocument.cpp",
        "bookmarksdocument.h",
        "bookmarkseditor.cpp",
        "bookmarkseditor.h",
        "bookmarksmenucontainer.cpp",
        "bookmarksmenucontainer.h",
        "bookmarksplugin.cpp",
        "bookmarksplugin.h",
        "bookmarksplugin_p.h",
        "bookmarksplugin.qrc",
        "bookmarkstoolmodel.cpp",
        "bookmarkstoolmodel.h",
        "bookmarkstoolwidget.cpp",
        "bookmarkstoolwidget.h",
        "bookmarkstoolwidget_p.h"
    ]
}
