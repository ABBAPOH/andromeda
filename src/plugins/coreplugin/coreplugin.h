#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <iplugin.h>
#include "core.h"

using namespace ExtensionSystem;

class CorePlugin : public IPlugin
{
    Q_OBJECT
public:
    CorePlugin();

    virtual bool initialize();
    virtual QString property(Properties property);
    virtual void shutdown();

signals:

public slots:

private:
    Core::Core *core;
};

#endif // COREPLUGIN_H
