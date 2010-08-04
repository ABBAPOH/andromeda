#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H

#include "pluginmanager.h"

namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate
{
public:
    PluginManagerPrivate();
    QString pluginsFolder;
    QList<PluginSpec *> pluginSpecs;

    QList<PluginSpec *> loadQueue();
    void loadSpecs();
    void resolveDependencies();
    void specFromPlugin(QObject * object);
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_P_H
