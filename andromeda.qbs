import qbs.base 1.0

Project {

    property string app_target: qbs.targetOS == "mac" ? "Andromeda" : "andromeda"
    property string install_app_path: qbs.targetOS == "mac" ? "" : "bin"
    property string install_binary_path: {
        if (qbs.targetOS == "mac")
            return app_target + ".app/Contents/MacOS"
        else
            return install_app_path
    }
    property string lib_suffix: qbs.targetOS == "linux" ? (qbs.architecture == "x86_64" ? "64" : "") : ""
    property string install_library_path: {
        if (qbs.targetOS == "mac")
            return app_target + ".app/Contents/Frameworks"
        else if (qbs.targetOS == "windows")
            return install_app_path
        else
            return "lib" + lib_suffix + "/" + app_target
    }
    property string install_plugin_path: {
        if (qbs.targetOS == "mac")
            return app_target + ".app/Contents/PlugIns"
        else if (qbs.targetOS == "windows")
            return "plugins"
        else
            return install_library_path + "/plugins"
    }
    property string install_data_path: {
        if (qbs.targetOS == "mac")
            return app_target + ".app/Contents/Resources"
        else if (qbs.targetOS == "windows")
            return "resources"
        else
            return "share/" + app_target
    }
    property string installNamePrefix: "@executable_path/../Frameworks/"

    references: [
        "libs/src/3rdparty/qtsingleapplication/qtsingleapplication.qbs",
        "libs/src/extensionsystem/extensionsystem.qbs",
        "libs/src/filemanager/filemanager.qbs",
        "libs/src/imageviewer/imageviewer.qbs",
        "libs/src/io/io.qbs",
        "libs/src/parts/parts.qbs",
        "libs/src/widgets/widgets.qbs",
        "libs/tests/extensionsystem/pluginspec/pluginspec.qbs",
        "libs/tests/filemanager/filemanagerwidget/filemanagerwidget.qbs",
        "libs/tests/io/kdesettings/kdesettings.qbs",
        "libs/tests/parts/sharedproperties/sharedproperties.qbs",
        "src/app/app.qbs",
        "src/plugins/parts/bineditorpart/bineditorpart.qbs",
        "src/plugins/parts/bookmarkspart/bookmarkspart.qbs",
        "src/plugins/parts/filemanagerpart/filemanagerpart.qbs",
        "src/plugins/parts/helloworldpart/helloworldpart.qbs",
        "src/plugins/parts/imageviewerpart/imageviewerpart.qbs",
        "src/plugins/parts/texteditorpart/texteditorpart.qbs",
        "src/plugins/parts/webviewpart/webviewpart.qbs"
    ]

    Product {
        name: "translations"
        type: "qm"
        Depends { name: "Qt.core" }
        files: [
            "libs/src/extensionsystem/translations/*.ts",
            "libs/src/filemanager/translations/*.ts",
            "libs/src/imageviewer/translations/*.ts",
            "libs/src/io/translations/*.ts",
            "libs/src/parts/translations/*.ts",
            "libs/src/widgets/translations/*.ts",
            "src/app/translations/*.ts",
            "src/plugins/parts/bineditorpart/translations/*.ts",
            "src/plugins/parts/bookmarkspart/translations/*.ts",
            "src/plugins/parts/filemanagerpart/translations/*.ts",
            "src/plugins/parts/helloworldpart/translations/*.ts",
            "src/plugins/parts/imageviewerpart/translations/*.ts",
            "src/plugins/parts/texteditorpart/translations/*.ts",
            "src/plugins/parts/webviewpart/translations/*.ts"
        ]

        Group {
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: install_data_path + "/translations"
        }
    }
}
