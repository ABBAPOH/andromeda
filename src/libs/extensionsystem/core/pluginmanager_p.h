#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H

#include "qobjectpool_p.h"
#include "options.h"
#include "pluginspecformathandler_p.h"

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class QFileSystemWatcher;
class QTranslator;

namespace ExtensionSystem {

class PluginSpec;

class PluginManager;
class PluginManagerPrivate : public QObjectPoolPrivate
{
    PluginManager *q_ptr;

    Q_DECLARE_PUBLIC(PluginManager)

public:
    PluginManagerPrivate(PluginManager *qq) : q_ptr(qq) {}

    bool load();
    QStringList getSpecFiles(QStringList folders);
    QList<PluginSpec *> loadSpecs(QStringList specFiles);

    void fileChanged(const QString &libraryPath);

    void loadLibsTranslations();
    void loadPluginsTranslations(const QStringList &specFiles);
    void loadTranslations(const QStringList &translations);
    void unloadTranslations();

    void enableSpecs(QList<PluginSpec *> specs);

    QList<PluginSpecFormatHandler*> handlers() const { return formatHandlers.toList(); }
    PluginSpecFormatHandler* handler(PluginSpec::Format f) const { return formatHandlers[f]; }

    Options &options() { return opts; }
    QVariantMap options(const QString &name);

    void clearError();
    void addErrorString(const QString &message);

public:
    QFileSystemWatcher *watcher;
    int updateTimer;

    QString pluginsFolder;
    QString translationsDir;
    QStringList translations;
    QList<QTranslator *> translators;

    bool hasErrors;
    QStringList errors;

    bool loaded;
    QList<PluginSpec *> pluginSpecs; // contains all specs

    QHash<QString, PluginSpec*> pathToSpec; // maps file to spec
    QStringList foldersToBeLoaded; // folders to be loaded on startup or after watcher event

    QVector<PluginSpecFormatHandler*> formatHandlers;

    Options opts;

    QStringList defaultPlugins;
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_P_H
