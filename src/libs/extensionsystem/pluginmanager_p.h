#ifndef PLUGINMANAGER_P_H
#define PLUGINMANAGER_P_H

#include "pluginmanager.h"
#include <QtCore/QHash>
#include <QtCore/QStringList>

class QFileSystemWatcher;
namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate
{
public:
    PluginManagerPrivate();
    QFileSystemWatcher *watcher;
    int updateTimer;

    QString pluginsFolder;
    QList<PluginSpec *> pluginSpecs; // contains all specs

    QObjectList objects;
    QHash<QString, QObject *> namedObjects;

    QHash<QString, PluginSpec*> pathToSpec; // maps file to spec
    QStringList foldersToBeLoaded; // folders to be loaded on startup or after watcher event

    void load();
    QStringList getSpecFiles(QStringList folders);
    QList<PluginSpec *> loadSpecs(QStringList specFiles);

    void fileChanged(const QString &libraryPath);

    void enableSpecs(QList<PluginSpec *> specs);
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_P_H
