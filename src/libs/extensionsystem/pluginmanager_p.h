#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H

#include "qobjectpool_p.h"

#include <QtCore/QHash>
#include <QtCore/QStringList>

class QFileSystemWatcher;
namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate : public QObjectPoolPrivate
{
public:
    PluginManagerPrivate();
    QFileSystemWatcher *watcher;
    int updateTimer;

    QString pluginsFolder;
    bool loaded;
    QList<PluginSpec *> pluginSpecs; // contains all specs

    QHash<QString, PluginSpec*> pathToSpec; // maps file to spec
    QStringList foldersToBeLoaded; // folders to be loaded on startup or after watcher event

    bool load();
    QStringList getSpecFiles(QStringList folders);
    QList<PluginSpec *> loadSpecs(QStringList specFiles);

    void fileChanged(const QString &libraryPath);

    void enableSpecs(QList<PluginSpec *> specs);
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_P_H
