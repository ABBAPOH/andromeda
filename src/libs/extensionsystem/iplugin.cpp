#include "iplugin.h"
#include "iplugin_p.h"

#include "pluginspec_p.h"
#include "pluginmanager.h"

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
void IPlugin::addObject(QObject * object)
{
    Q_D(IPlugin);

    if (!d->addedObjects.contains(object))
        d->addedObjects.append(object);

    PluginManager::instance()->addObject(object);
}

/*!
    \fn void IPlugin::addObjectByName(QObject * object, const QString name)
    \brief Adds \a object to object pool in PluginManager and sets its objectName to \a name.
*/
void IPlugin::addObjectByName(QObject * object, const QString name)
{
    object->setObjectName(name);
    addObject(object);
}

/*!
    \fn void IPlugin::addObjectByType(QObject * object, const QString type)
    \brief Adds \a object to object pool in PluginManager.

    \a type can be used to determine who should handle this object.
*/
void IPlugin::addObjectByType(QObject * object, const QString type)
{
    Q_D(IPlugin);

    if (!d->addedObjects.contains(object))
        d->addedObjects.append(object);

    PluginManager::instance()->addObject(object, type);
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

// ============= IPluginPrivate =============

IPluginPrivate::IPluginPrivate()
{
}

IPluginPrivate::~IPluginPrivate()
{
    foreach (QObject *object, addedObjects) {
        PluginManager::instance()->removeObject(object);
        delete object;
    }
}

