#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QtGui/QDesktopServices>

#include <iplugin.h>
#include <GuiSystem>

using namespace ExtensionSystem;

class CorePluginImpl : public IPlugin
{
    Q_OBJECT
public:
    CorePluginImpl();

    bool initialize();
    void shutdown();

public slots:
    void newWindow();
    void showPluginView();
    void handleMessage(const QString &);

private:
    void createActions();

    void createGotoDirCommand(
                              GuiSystem::CommandContainer * container,
                              QDesktopServices::StandardLocation location,
                              const QIcon &icon = QIcon()
                            );
};

#endif // COREPLUGIN_H
