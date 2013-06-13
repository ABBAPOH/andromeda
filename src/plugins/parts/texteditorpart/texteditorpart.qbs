import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "TextEditorPart"

    Depends { name: "Qt"; submodules: ["core", "widgets"] }

    files : [
        "plaintextdocument.cpp",
        "plaintextdocument.h",
        "plaintextedit.cpp",
        "plaintextedit.h",
        "plaintexteditor.cpp",
        "plaintexteditor.h",
        "texteditorplugin.cpp",
        "texteditorplugin.h",
        "texteditorplugin.qrc",
        "textfind.cpp",
        "textfind.h"
    ]
}
