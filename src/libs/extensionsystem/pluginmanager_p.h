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

    void loadSpecs();
    void specFromPlugin(QObject * object);
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_P_H
