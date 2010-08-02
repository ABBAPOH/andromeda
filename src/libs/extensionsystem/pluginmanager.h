#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

#include "pluginspec.h"

namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PluginManager)
    Q_DECLARE_PRIVATE(PluginManager)
    Q_PROPERTY(QString pluginsFolder READ pluginsFolder WRITE setPluginsFolder)

public:
    static PluginManager *instance();

    explicit PluginManager(QObject *parent = 0);
    virtual ~PluginManager();

    void addObject(QObject * object);
    void removeObject(QObject * object);

    void loadPlugins();
    void shutdown();

    QString pluginsFolder();
    void setPluginsFolder(const QString &name);

    QList<PluginSpec *> plugins() const;

signals:

public slots:

private:
    PluginManagerPrivate *d_ptr;
    static PluginManager *m_instance;
    mutable QReadWriteLock m_lock;
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
