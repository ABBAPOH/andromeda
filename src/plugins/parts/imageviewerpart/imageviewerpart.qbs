import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "ImageViewerPart"

    Depends { name: "Qt"; submodules: ["core", "widgets"] }
    Depends { name: "ImageViewer" }
    Depends { name: "Widgets" }

    files : [
        "imageviewerdocument.cpp",
        "imageviewerdocument.h",
        "imageviewereditor.cpp",
        "imageviewereditor.h",
        "imageviewerplugin.cpp",
        "imageviewerplugin.h",
        "imageviewerplugin.qrc"
    ]
}
