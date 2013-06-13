import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "HelloWorldPart"

    Depends { name: "Qt"; submodules: ["core", "widgets"] }

    files : [
        "helloworlddocument.cpp",
        "helloworlddocument.h",
        "helloworldeditor.cpp",
        "helloworldeditor.h",
        "helloworldplugin.cpp",
        "helloworldplugin.h",
        "helloworldplugin.qrc"
    ]
}
