#include "application.h"

#include <QtCore/QDir>

#ifndef QT_WEBKIT_NOT_FOUND
#include <QtWebKit/QWebSecurityOrigin>
#endif

using namespace ExtensionSystem;

// we need this to prevent libraries from unloading when they are not used
void preloadLibraries()
{
#ifndef QT_WEBKIT_NOT_FOUND
    QStringList schemes = QWebSecurityOrigin::localSchemes(); // preloading WebKit
#endif
}

int main(int argc, char *argv[])
{
    Application app(argc, argv);

    if (app.isRunning()) {
        if (!app.activateApplication())
            return 2;
        return 0;
    }

    preloadLibraries();

    if (!app.loadPlugins())
        return 1;

    return app.exec();
}
