import qbs.base 1.0

Project {

    id: andromeda
    property string app_target: qbs.targetOS.contains("osx") ? "Andromeda" : "andromeda"
    property string install_app_path: qbs.targetOS.contains("osx") ? "" : "bin"
    property string install_binary_path: {
        if (qbs.targetOS.contains("osx"))
            return app_target + ".app/Contents/MacOS"
        else
            return install_app_path
    }
    property string lib_suffix: ""
    property string install_library_path: {
        if (qbs.targetOS.contains("osx"))
            return app_target + ".app/Contents/Frameworks"
        else if (qbs.targetOS.contains("windows"))
            return install_app_path
        else
            return "lib" + lib_suffix + "/" + app_target
    }
    property string install_plugin_path: {
        if (qbs.targetOS.contains("osx"))
            return app_target + ".app/Contents/PlugIns"
        else if (qbs.targetOS.contains("windows"))
            return "plugins"
        else
            return install_library_path + "/plugins"
    }
    property string install_data_path: {
        if (qbs.targetOS.contains("osx"))
            return app_target + ".app/Contents/Resources"
        else if (qbs.targetOS.contains("windows"))
            return "resources"
        else
            return "share/" + app_target
    }
    property string installNamePrefix: "@executable_path/../Frameworks/"

    SubProject {
        filePath: "libs/libs.qbs"
        Properties {
            app_target: andromeda.app_target
            install_binary_path: andromeda.install_binary_path
            install_library_path: andromeda.install_library_path
            install_plugin_path: andromeda.install_plugin_path
            install_data_path: andromeda.install_data_path
            installNamePrefix: andromeda.installNamePrefix
        }
    }

    SubProject {
        filePath: "src/src.qbs"
    }
}
