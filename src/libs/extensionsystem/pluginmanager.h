#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "qobjectpool.h"
#include "pluginspec.h"

namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObjectPool
{
    Q_OBJECT
    Q_DISABLE_COPY(PluginManager)
    Q_DECLARE_PRIVATE(PluginManager)
    Q_PROPERTY(QString pluginsFolder READ pluginsFolder WRITE setPluginsFolder)

public:
    explicit PluginManager(QObject *parent = 0);
    virtual ~PluginManager();

    static PluginManager *instance();

    void loadPlugins();
    void shutdown();

    QString pluginsFolder() const;
    void setPluginsFolder(const QString &name);

    QList<PluginSpec *> plugins() const;

signals:
    void pluginsChanged();

public slots:
    void updateDirectory(const QString &);
    void updateLibrary(const QString &);

protected:
    void timerEvent(QTimerEvent *event);

private:
    static PluginManager *m_instance;
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
