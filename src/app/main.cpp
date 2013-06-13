#include "application.h"

#include <QtCore/QDir>

#ifndef QT_WEBKIT_NOT_FOUND
#include <QtWebKit/QWebSecurityOrigin>
#endif

#include <IO/QMimeDatabase>

using namespace ExtensionSystem;
using namespace Andromeda;

// we need this to prevent libraries from unloading when they are not used
void preloadLibraries()
{
#ifndef QT_WEBKIT_NOT_FOUND
    QStringList schemes = QWebSecurityOrigin::localSchemes(); // preloading WebKit
#endif

    QMimeDatabase db;
    db.mimeTypeForName("application/octet-stream");
}

int main(int argc, char *argv[])
{
    Application app(argc, argv);

#if QT_VERSION >= 0x050000
#else
    if (app.isRunning()) {
        if (!app.activateApplication())
            return 2;
        return 0;
    }
#endif

    preloadLibraries();

    if (!app.loadPlugins())
        return 1;

    return app.exec();
}
