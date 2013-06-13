import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "BinEditorPart"

    Depends { name : "Qt"; submodules: ["core", "widgets"] }
    cpp.defines: [ "BINEDITORPART_LIBRARY" ]

    files : [
        "binedit.cpp",
        "binedit.h",
        "bineditor.cpp",
        "bineditor.h",
        "bineditor_global.h",
        "bineditordocument.cpp",
        "bineditordocument.h",
        "bineditorplugin.cpp",
        "bineditorplugin.h",
        "bineditorplugin.qrc"
    ]
}
