#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include "extensionsystem_global.h"

#include <QtCore/QObject>

class QIODevice;

namespace ExtensionSystem {

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

    bool operator==(const Version &) const;
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

class IPlugin;

class PluginSpecPrivate;
class EXTENSIONSYSTEM_EXPORT PluginSpec : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginSpec)
    Q_DISABLE_COPY(PluginSpec)

    Q_PROPERTY(bool loaded READ loaded WRITE setLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool loadOnStartup READ loadOnStartup WRITE setLoadOnStartup NOTIFY loadOnStartupChanged)
    Q_PROPERTY(bool canBeUnloaded READ canBeUnloaded)

public:
    enum Format { TextFormat, BinaryFormat };

    explicit PluginSpec(QObject *parent = 0);
    explicit PluginSpec(const QString & path);
//    explicit PluginSpec(QIODevice *device);
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

    QString errorString() const;
    bool hasError() const;

    IPlugin *plugin() const;

    bool provides(const PluginDependency &dependency) const;

    bool read(const QString & path);
//    bool read(QIODevice *device);
    bool write(const QString & path, Format format = TextFormat);
//    bool write(QIODevice *device, Format format);

public slots:
    void load();
    void unload();
    void setLoaded(bool yes);
    void setLoadOnStartup(bool yes);

signals:
    void loadedChanged(bool loaded);
    void loadOnStartupChanged(bool loads);
    void error(const QString &message);

protected:
    PluginSpecPrivate *d_ptr;
};

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
