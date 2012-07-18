#include "iplugin.h"

#include "pluginmanager.h"

namespace ExtensionSystem {

class IPluginPrivate
{
public:
    QList<QObject *> addedObjects;
};

} // namespace ExtensionSystem

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::IPlugin
    \brief IPlugin is a base class for user plugins.
*/

/*!
    \brief Creates an IPlugin.
*/
IPlugin::IPlugin() :
    QObject(),
    d_ptr(new IPluginPrivate)
{
}

/*!
    \brief Destroys IPlugin.
*/
IPlugin::~IPlugin()
{
    Q_D(IPlugin);

    for (int i = d->addedObjects.count() - 1; i >= 0; i--) {
        QObject *object = d->addedObjects[i];
        PluginManager::instance()->removeObject(object);
        delete object;
    }

    delete d_ptr;
}

/*!
    \brief Reiplement to perform primary initialization of a plugin.

    This function called after plugin was loaded and after all dependencies are initialized.
*/

/*!
    \brief Reiplement to perform secondary initialization of a plugin.

    Passed \a options are the command line arguments for this plugin. They only
    include keys described in plugin's spec file.

    For a single-instance applications, this function is called for a first
    instance each time new instance is trying to start (i.e. when QtSingleApplication
    receives message from new instance).
*/

/*!
    \brief Reiplement to perform deinitialization of a plugin.

    This function called before plugin will be unloaded.
*/

QObject *IPlugin::object(const QString &name)
{
    return PluginManager::instance()->object(name);
}

/*!
    \brief Adds \a object to object pool in PluginManager.
*/
void IPlugin::addObject(QObject * object, const QString &name)
{
    Q_D(IPlugin);

    if (!d->addedObjects.contains(object))
        d->addedObjects.append(object);

    PluginManager::instance()->addObject(object, name);
}

/*!
    \brief Removes \a object from object pool in PluginManager.
*/
void IPlugin::removeObject(QObject * object)
{
    Q_D(IPlugin);

    d->addedObjects.removeAll(object);
    PluginManager::instance()->removeObject(object);
}
