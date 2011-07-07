#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <iplugin.h>

using namespace ExtensionSystem;

class CorePluginImpl : public IPlugin
{
    Q_OBJECT
public:
    CorePluginImpl();

    virtual bool initialize();
    virtual void shutdown();

public slots:
    void newWindow();

private:
    void createActions();};

#endif // COREPLUGIN_H
