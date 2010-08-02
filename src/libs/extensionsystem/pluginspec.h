#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include "extensionsystem_global.h"
#include <QtCore/QString>
#include <QObject>
#include <QMetaType>

namespace ExtensionSystem {
class IPlugin;
class PluginSpecPrivate;

class EXTENSIONSYSTEM_EXPORT PluginDependency
{
    QString m_name;
    QString m_version;
public:
    PluginDependency(const QString &name, const QString &version);
    inline QString name() { return m_name; }
    inline QString version() { return m_version; }
    bool operator==(const PluginDependency &other);
};

class EXTENSIONSYSTEM_EXPORT PluginSpec
{
    Q_DECLARE_PRIVATE(PluginSpec)

public:
    ~PluginSpec();

    QString name() const;
    QString version() const;
    QString compatibilityVersion() const;
    QString vendor() const;
    QList<PluginDependency> dependencies() const;
    QList<PluginSpec*> dependencySpecs() const;

    QString libraryPath() const; // path to library
    bool isStatic() const;

    QString errorString();
    bool hasError();

    IPlugin *plugin() const;

    bool provides(const QString &pluginName, const QString &pluginVersion) const;

protected:
    PluginSpec();
    PluginSpec(IPlugin * plugin, const QString & path);
    explicit PluginSpec(IPlugin * plugin);

    PluginSpecPrivate *d_ptr;
//    friend class PluginManager;
    friend class PluginManagerPrivate;
    friend class IPlugin;
};

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
