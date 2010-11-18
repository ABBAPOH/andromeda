#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include "extensionsystem_global.h"
#include <QtCore/QString>
#include <QObject>
#include <QMetaType>

namespace ExtensionSystem {
class IPlugin;
class PluginSpecPrivate;

class EXTENSIONSYSTEM_EXPORT Version
{
public:
    unsigned major;
    unsigned minor;
    unsigned build;
    unsigned revision;

    Version();
    explicit Version(const QString &);

    QString toString() const;
    static Version fromString(const QString &);
    bool operator==(const Version &);
};

class EXTENSIONSYSTEM_EXPORT PluginDependency
{
    QString m_name;
    Version m_version;

public:
    PluginDependency() {}
    PluginDependency(const QString &name, const QString &version);
    inline QString name() const { return m_name; }
    inline Version version() const { return m_version; }
    bool operator==(const PluginDependency &other);

};

class EXTENSIONSYSTEM_EXPORT PluginSpec : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginSpec)
    Q_DISABLE_COPY(PluginSpec)
    Q_PROPERTY(bool loaded READ loaded WRITE setLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool loadOnStartup READ loadOnStartup WRITE setLoadOnStartup NOTIFY loadOnStartupChanged)
    Q_PROPERTY(bool canBeUnloaded READ canBeUnloaded)

public:
    PluginSpec();
    explicit PluginSpec(const QString & path);
    ~PluginSpec();

    QString name() const;
    Version version() const;
    Version compatibilityVersion() const;
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

    bool loaded() const;
    bool loadOnStartup() const;
    bool canBeUnloaded() const;

    QString errorString();
    bool hasError();

    IPlugin *plugin() const;

    bool provides(const PluginDependency &dependency) const;

public slots:
    void load();
    void unload();
    void setLoaded(bool yes);
    void setLoadOnStartup(bool yes);

signals:
    void loadedChanged(bool loaded);
    void loadsOnStartupChanged(bool loads);
    void error(const QString &message);

protected:
    PluginSpecPrivate *d_ptr;

    friend QDataStream &operator>>(QDataStream &s, PluginSpec &pluginSpec);
    friend QDataStream &operator<<(QDataStream &s, const PluginSpec &pluginSpec);
};

QDataStream & operator>>(QDataStream &s, PluginSpec &pluginSpec);
QDataStream & operator<<(QDataStream &s, const PluginSpec &pluginSpec);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
