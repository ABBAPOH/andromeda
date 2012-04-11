#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "qobjectpool.h"
#include "pluginspec.h"

#include <QtCore/QStringList>

namespace ExtensionSystem {

class PluginManagerPrivate;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObjectPool
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginManager)
    Q_DISABLE_COPY(PluginManager)

    Q_PROPERTY(bool hasErrors READ hasErrors)
    Q_PROPERTY(QStringList errors READ errors)
    Q_PROPERTY(bool loaded READ loaded)
    Q_PROPERTY(QStringList defaultPlugins READ defaultPlugins WRITE setDefaultPlugins)
    Q_PROPERTY(QString pluginsFolder READ pluginsFolder WRITE setPluginsFolder)
    Q_PROPERTY(QString translationsDir READ translationsDir WRITE setTranslationsDir)

public:
    explicit PluginManager(QObject *parent = 0);
    ~PluginManager();

    static PluginManager *instance();

    QStringList defaultPlugins() const;
    void setDefaultPlugins(const QStringList &plugins);

    bool hasErrors() const;
    QStringList errors() const;

    bool loaded();

    QString pluginsFolder() const;
    void setPluginsFolder(const QString &name);

    QString translationsDir() const;
    void setTranslationsDir(const QString &dir);

    QStringList translations() const;
    void setTranslations(const QStringList &translations);

    QList<PluginSpec *> plugins() const;
    PluginSpec *plugin(const QString &name) const;

signals:
    void pluginsChanged();

    void pluginsLoaded();
    void pluginsUnloaded();
    void error(const QString &errors);

public slots:
    void loadPlugins();
    void postInitialize(const QStringList &arguments);
    void unloadPlugins();

    void updateDirectory(const QString &);
    void updateLibrary(const QString &);

protected:
    void timerEvent(QTimerEvent *event);

private:
    static PluginManager *m_instance;

    friend class PluginSpec;
    friend class PluginSpecPrivate;
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
