import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "FileManagerPart"

    Depends { name: "Qt"; submodules: ["core", "widgets"] }
    Depends { name: "FileManager" }
    Depends { name: "IO" }
    Depends { name: "Widgets" }

    files : [
        "filemanagerdocument.cpp",
        "filemanagerdocument.h",
        "filemanagereditor.cpp",
        "filemanagereditor.h",
        "filemanagereditor_p.h",
        "filemanagerplugin.cpp",
        "filemanagerplugin.h",
        "filemanagerplugin.qrc",
        "filesystemtoolmodel.cpp",
        "filesystemtoolmodel.h",
        "filesystemtoolwidget.cpp",
        "filesystemtoolwidget.h",
        "filesystemtoolwidget_p.h",
        "globalsettings.cpp",
        "globalsettings.h",
        "globalsettings.ui",
        "openwitheditormenu.cpp",
        "openwitheditormenu.h",
        "viewmodessettings.cpp",
        "viewmodessettings.h",
        "viewmodessettings.ui"
    ]
}
