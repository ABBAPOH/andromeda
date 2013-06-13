import qbs.base 1.0
import "../part.qbs" as Part

Part {
    name : "WebViewPart"

    Depends { id: qtcore; name: "Qt.core"; }
    Depends {
        name: "Qt";
        submodules: qtcore.versionMajor < 5 ?
                        ["widgets", "network", "webkit"] :
                        ["widgets", "network", "webkit", "webkitwidgets"]
    }
    Depends { name: "IO" }
    Depends { name: "Widgets" }

    files : [
        "appearancesettings.cpp",
        "appearancesettings.h",
        "appearancesettings.ui",
        "cookiedialog.cpp",
        "cookiedialog.h",
        "cookiedialog.ui",
        "cookieexceptionsdialog.cpp",
        "cookieexceptionsdialog.h",
        "cookieexceptionsdialog.ui",
        "cookieexceptionsmodel.cpp",
        "cookieexceptionsmodel.h",
        "cookiejar.cpp",
        "cookiejar.h",
        "cookiemodel.cpp",
        "cookiemodel.h",
        "privacysettings.cpp",
        "privacysettings.h",
        "privacysettings.ui",
        "proxysettings.cpp",
        "proxysettings.h",
        "proxysettings.ui",
        "webviewconstants.h",
        "webviewdocument.cpp",
        "webviewdocument.h",
        "webvieweditor.cpp",
        "webvieweditor.h",
        "webvieweditor_p.h",
        "webviewplugin.cpp",
        "webviewplugin.h"
    ]
}
