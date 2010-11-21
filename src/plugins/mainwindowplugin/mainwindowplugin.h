#ifndef MAINWINDOWPLUGIN_H
#define MAINWINDOWPLUGIN_H

#include <iplugin.h>

class MainWindow;
namespace MainWindowPlugin {

class MainWindowPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    explicit MainWindowPluginImpl();

    virtual bool initialize();
    virtual void shutdown();

    MainWindow *window;

signals:

public slots:

};

}

#endif // MAINWINDOWPLUGIN_H
