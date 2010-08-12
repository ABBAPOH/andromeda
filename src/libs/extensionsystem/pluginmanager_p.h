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
    QString pluginsFolder;
    QList<PluginSpec *> pluginSpecs;
    QList<PluginSpec *> specsToBeEnabled;
    QFileSystemWatcher *watcher;
    QHash<QString, PluginSpec*> pathToSpec;
    QObjectList objects;
    QHash<QString, QObject *> namedObjects;

    QStringList pathsToBeUpdated;
    int updateTimer;

    void loadSpecs();
    void specFromPlugin(QObject * object);
    void fileChanged(const QString &libraryPath);
    void parseDirectory(const QString &dir);
    void enableSpecs();
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_P_H
