#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "qobjectpool.h"
#include "pluginspec.h"

namespace ExtensionSystem {

class PluginManagerPrivate;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObjectPool
{
    Q_OBJECT
    Q_DISABLE_COPY(PluginManager)
    Q_DECLARE_PRIVATE(PluginManager)
    Q_PROPERTY(bool loaded READ loaded)
    Q_PROPERTY(QString pluginsFolder READ pluginsFolder WRITE setPluginsFolder)

public:
    explicit PluginManager(QObject *parent = 0);
    virtual ~PluginManager();

    static PluginManager *instance();

    bool loaded();

    QString pluginsFolder() const;
    void setPluginsFolder(const QString &name);

    QList<PluginSpec *> plugins() const;
    PluginSpec *plugin(const QString &name) const;

signals:
    void pluginsChanged();

    void pluginsLoaded();
    void pluginsUnloaded();

public slots:
    void loadPlugins();
    void unloadPlugins();

    void updateDirectory(const QString &);
    void updateLibrary(const QString &);

protected:
    void timerEvent(QTimerEvent *event);

private:
    static PluginManager *m_instance;
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
