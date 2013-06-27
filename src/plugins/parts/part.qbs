import qbs.base 1.0

DynamicLibrary {
    destinationDirectory: install_plugin_path

    Depends { name: "cpp" }
    Depends { name: "ExtensionSystem" }
    Depends { name: "Parts" }

    cpp.includePaths : [ "../../../../libs/include" ]
    cpp.dynamicLibraryPrefix: "lib"

    Properties {
        condition: qbs.targetOS.contains("osx")
        cpp.installNamePrefix: "@executable_path/../Frameworks/"
    }

    Properties {
         condition: qbs.targetOS == "linux" || qbs.targetOS == "unix"
         cpp.rpaths: "$ORIGIN/../.."
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_plugin_path + "/andromeda"
    }
    Group {
        name: "pluginspec"
        files: product.name + ".spec"
        qbs.install: true
        qbs.installDir: project.install_plugin_path + "/andromeda"
    }
}
