#ifndef FILEMANAGERPLUGIN_H
#define FILEMANAGERPLUGIN_H

#include <iplugin.h>

#include <QtGui/QDesktopServices>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

class FileManagerPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit FileManagerPluginImpl(QObject *parent = 0);

    bool initialize();
    void shutdown();
    void createGotoDirCommand(QDesktopServices::StandardLocation location,
                              const QIcon &icon = QIcon(),
                              const QKeySequence &key = QKeySequence());
};

#endif // FILEMANAGERPLUGIN_H
