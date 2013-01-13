#ifndef PLUGINSPEC_P_H
#define PLUGINSPEC_P_H

#include "pluginspec.h"
#include "iplugin.h"

#include "options.h"

class QPluginLoader;

namespace ExtensionSystem {

class PluginSpecPrivate
{
    Q_DECLARE_PUBLIC(PluginSpec)
    PluginSpec *q_ptr;

public:
    PluginSpecPrivate(PluginSpec *qq);

    IPlugin *plugin;
    QPluginLoader *loader;

    QString name;
    Version version;
    Version compatibilityVersion;
    QString vendor;
    QString category;
    QString copyright;
    QString license;
    QString description;
    QString url;

    QList<PluginDependency> dependencies;

    QList<PluginSpec*> dependencySpecs;
    QList<PluginSpec*> dependentSpecs;

    QString libraryPath;
    QString libraryLocation;

    bool loaded;
    bool loadOnStartup;
    bool isDefault;

    QString errorString;
    bool hasError;

    QList<Option> options;
    QString defaultOption;

    bool load();
    bool loadLibrary();
    bool resolveDependencies();
    bool unload();
    bool unloadLibrary();
    static int compareVersion(const Version &version1, const Version &version2);
    QString getLibraryPath(const QString &path);
    void setError(const QString &message);
    void clearError() { errorString.clear(); hasError = false; }
};

QDataStream & operator>>(QDataStream &s, PluginSpecPrivate &pluginSpec);
QDataStream & operator<<(QDataStream &s, const PluginSpecPrivate &pluginSpec);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_P_H
