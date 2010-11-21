#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <iplugin.h>
#include "core.h"

using namespace ExtensionSystem;

class CorePluginImpl : public IPlugin
{
    Q_OBJECT
public:
    CorePluginImpl();

    virtual bool initialize();
    virtual void shutdown();

signals:

public slots:

private:
    CorePlugin::Core *core;
};

#endif // COREPLUGIN_H
