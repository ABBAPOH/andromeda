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
    \fn IPlugin::IPlugin()
    \internal
*/
IPlugin::IPlugin() :
    QObject(),
    d_ptr(new IPluginPrivate)
{
}

/*!
    \fn IPlugin::~IPlugin()
    \internal
*/
IPlugin::~IPlugin()
{
    Q_D(IPlugin);

    foreach (QObject *object, d->addedObjects) {
        PluginManager::instance()->removeObject(object);
        delete object;
    }

    delete d_ptr;
}

/*!
    \fn bool IPlugin::initialize()
    \brief Function for primary initialization of plugin.
    This function called after all dependencies are initialized.
*/

/*!
    \fn void addObject(QObject * object)
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
    \fn void removeObject(QObject * object)
    \brief Removes \a object from object pool in PluginManager.
*/
void IPlugin::removeObject(QObject * object)
{
    Q_D(IPlugin);

    d->addedObjects.removeAll(object);
    PluginManager::instance()->removeObject(object);
}
