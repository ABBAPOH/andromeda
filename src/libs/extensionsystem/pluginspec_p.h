#ifndef PLUGINSPEC_P_H
#define PLUGINSPEC_P_H

#include "iplugin.h"

namespace ExtensionSystem {
    class PluginSpecPrivate
    {
    public:
        PluginSpecPrivate();

        IPlugin *plugin;
        QString name;
        QString version;
        QString compatibilityVersion;
        QString vendor;
        QList<PluginDependency> dependencies;
        QList<PluginSpec*> dependencySpecs;

        QString libraryPath;
        QString libraryLocation;
        bool isStatic;

        void init(IPlugin * plugin);
        void resolveDependencies(const QList<PluginSpec*> &specs);
    };
} // namespace ExtensionSystem


#endif // PLUGINSPEC_P_H
