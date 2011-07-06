#ifndef MAINWINDOWPLUGIN_H
#define MAINWINDOWPLUGIN_H

#include <iplugin.h>

namespace MainWindowPlugin {

class MainWindowPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    explicit MainWindowPluginImpl();

    virtual bool initialize();
    virtual void shutdown();

public slots:
    void newWindow();

private:
    void createActions();
};

}

#endif // MAINWINDOWPLUGIN_H
