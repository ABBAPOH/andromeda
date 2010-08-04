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

class EXTENSIONSYSTEM_EXPORT PluginSpec : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginSpec)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    ~PluginSpec();

    QString name() const;
    QString version() const;
    QString compatibilityVersion() const;
    QString vendor() const;
    QString category() const;
    QString copyright() const;
    QString license() const;
    QString description() const;
    QString url() const;

    QList<PluginDependency> dependencies() const;
    QList<PluginSpec*> dependencySpecs() const;

    QString libraryPath() const; // path to library
    bool isStatic() const;

    void setEnabled(bool enabled);
    bool enabled() const;

    QString errorString();
    bool hasError();

    IPlugin *plugin() const;

    bool provides(const QString &pluginName, const QString &pluginVersion) const;

signals:
    void enabledChanged(bool enabled);

protected:
    PluginSpec();
    PluginSpec(const QString & path);
    explicit PluginSpec(IPlugin * plugin);

    PluginSpecPrivate *d_ptr;
//    friend class PluginManager;
    friend class PluginManagerPrivate;
    friend class IPlugin;
};

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
