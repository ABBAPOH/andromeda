#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <iplugin.h>

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
};

#endif // COREPLUGIN_H
