import qbs.base 1.0

Application {
    name : project.app_target
    consoleApplication: qbs.debugInformation

    Depends { name: "cpp" }
    Depends { id: qtcore; name: "Qt.core" }
    Depends { name: "Qt"; submodules: ["widgets", "webkit"] }
    Depends { name: "ExtensionSystem" }
    Depends { name: "IO" }
    Depends { name: "Parts" }
    Depends { name: "Widgets" }
    Depends { name: "QtSingleApplication" }
    cpp.includePaths : [
        "../../libs/include",
        "../../libs/src/3rdparty/qtsingleapplication/qtsingleapplication",
        "../../libs/src",
        "../../src/plugins/parts/"
    ]
    cpp.defines : [
        "PROJECT_VERSION=\"1\"",
        "GIT_REVISION=\"\"",
        "LIB_SUFFIX=\"" + project.lib_suffix + "\""
    ]

    Properties {
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        cpp.rpaths: [ "$ORIGIN/../lib" + project.lib_suffix + "/" + project.app_target ]
    }

    files : [
        "andromeda.qrc",
        "application.cpp",
        "application.h",
        "browsertabwidget.cpp",
        "browsertabwidget.h",
        "browserwindow.cpp",
        "browserwindow.h",
        "browserwindow_p.h",
        "commandssettingspage.cpp",
        "commandssettingspage.h",
        "generalsettingspage.cpp",
        "generalsettingspage.h",
        "generalsettingswidget.cpp",
        "generalsettingswidget.h",
        "generalsettingswidget.ui",
        "main.cpp",
        "myaddressbar.cpp",
        "myaddressbar.h",
        "settingsmodel.cpp",
        "settingsmodel.h",
        "settingsmodel_p.h",
        "settingswidget.cpp",
        "settingswidget.h",
        "settingswidget.ui"
    ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_app_path
    }
    Group {
        name: "andromeda.icns"
        condition: qbs.targetOS.contains("osx")
        files: "Andromeda.icns"
        qbs.install: true
        qbs.installDir: install_data_path
    }
    Group {
        name: "andromeda.rc"
        condition: qbs.targetOS.contains("windows")
        files: "andromeda.rc"
    }
    Group {
        name: "andromeda.desktop"
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        files: "andromeda.desktop"
        qbs.install: true
        qbs.installDir: "share/applications"
    }
    Group {
        name: "andromeda.png"
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        files: "andromeda.png"
        qbs.install: true
        qbs.installDir: "share/pixmaps"
    }
}
