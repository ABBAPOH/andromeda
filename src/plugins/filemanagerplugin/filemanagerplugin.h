#ifndef FILEMANAGERPLUGIN_H
#define FILEMANAGERPLUGIN_H

#include <iplugin.h>

#include <QtGui/QDesktopServices>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>
#include "navigationmodel.h"

class FileManagerPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit FileManagerPluginImpl(QObject *parent = 0);

    bool initialize();
    void shutdown();
private slots:
    void onStandardLocationsChanged(NavigationModel::StandardLocations);

private:
    void createActions();
    void createFileMenu();
    void createViewMenu();
    void createGoToMenu();
    void createGoToDirCommand(QDesktopServices::StandardLocation location,
                              const QIcon &icon = QIcon(),
                              const QKeySequence &key = QKeySequence());
};

#endif // FILEMANAGERPLUGIN_H
