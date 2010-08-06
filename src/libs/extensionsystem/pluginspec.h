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
    PluginDependency() {}
    PluginDependency(const QString &name, const QString &version);
    inline QString name() const { return m_name; }
    inline QString version() const { return m_version; }
    bool operator==(const PluginDependency &other);
};

class EXTENSIONSYSTEM_EXPORT PluginSpec : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginSpec)
    Q_DISABLE_COPY(PluginSpec)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool loadOnStartup READ loadOnStartup WRITE setLoadOnStartup NOTIFY loadOnStartupChanged)
    Q_PROPERTY(bool canBeUnloaded READ canBeUnloaded)

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
    // plugins that are requied for this plugin
    QList<PluginSpec*> dependencySpecs() const;
    // plugins that require this plugin
    QList<PluginSpec*> dependentSpecs() const;

    QString libraryPath() const; // path to library

    bool isStatic() const;
    bool enabled() const;
    bool loadOnStartup() const;
    bool canBeUnloaded() const;

    QString errorString();
    bool hasError();

    IPlugin *plugin() const;

    bool provides(const QString &pluginName, const QString &pluginVersion) const;

public slots:
    void setEnabled(bool enabled);
    void setLoadOnStartup(bool on);

signals:
    void enabledChanged(bool enabled);
    void loadsOnStartupChanged(bool on);

protected:
    PluginSpec();
    PluginSpec(const QString & path);
    explicit PluginSpec(IPlugin * plugin);

    PluginSpecPrivate *d_ptr;

    friend class PluginManagerPrivate;
    friend QDataStream &operator>>(QDataStream &s, PluginSpec &pluginSpec);
    friend QDataStream &operator<<(QDataStream &s, const PluginSpec &pluginSpec);
};

QDataStream & operator>>(QDataStream &s, PluginSpec &pluginSpec);
QDataStream & operator<<(QDataStream &s, const PluginSpec &pluginSpec);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
