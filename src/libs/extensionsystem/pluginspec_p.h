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
        QString category;
        QList<PluginDependency> dependencies;
        QList<PluginSpec*> dependencySpecs;

        QString libraryPath;
        QString libraryLocation;
        bool isStatic;

        bool enabled;

        QString errorString;
        bool hasError;

        bool loadLibrary();
        bool unloadLibrary();

        void init(IPlugin * plugin);
        bool resolveDependencies(const QList<PluginSpec*> &specs);
        static int compareVersion(const QString &version1, const QString &version2);
    };
} // namespace ExtensionSystem


#endif // PLUGINSPEC_P_H
