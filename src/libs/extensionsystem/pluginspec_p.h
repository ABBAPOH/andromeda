#ifndef PLUGINSPEC_P_H
#define PLUGINSPEC_P_H

#include "iplugin.h"

class QPluginLoader;
namespace ExtensionSystem {
    class PluginSpecPrivate
    {
    public:
        PluginSpecPrivate();

        IPlugin *plugin;
        QPluginLoader * loader;

        QString name;
        QString version;
        QString compatibilityVersion;
        QString vendor;
        QString category;
        QString copyright;
        QString license;
        QString description;
        QString url;

        QList<PluginDependency> dependencies;
        QList<PluginSpec*> dependencySpecs;

        QString libraryPath;
        QString libraryLocation;

        bool isStatic;
        bool enabled;
        bool loadsOnStartup;

        QString errorString;
        bool hasError;

        void init(IPlugin * plugin);
        bool load();
        bool loadLibrary();
        bool resolveDependencies();
        bool unload();
        bool unloadLibrary();
        static int compareVersion(const QString &version1, const QString &version2);
    };
} // namespace ExtensionSystem


#endif // PLUGINSPEC_P_H
