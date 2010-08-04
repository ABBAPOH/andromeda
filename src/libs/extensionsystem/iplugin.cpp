#include "iplugin.h"
#include "iplugin_p.h"

#include "pluginspec_p.h"

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
}

/*!
    \fn bool IPlugin::initialize()
    \brief Function for primary initialization of plugin.
    This function called after all dependencies are initialized.
*/

/*!
    \fn void extensionsInitialized()
    \brief Function for secondary initialization of plugin.
    This function called after both the IPlugin::initialize() and IPlugin::extensionsInitialized()
    methods of plugins that depend on this plugin have been called.
*/

/*!
    \fn QString property(Properties property)
    \brief Returns string propertis of the plugin.
    Information from this function is stored in PluginSpec class.
    This is place where you have to place all string information for plugin.
    \sa PluginSpec
    \sa IPlugin::Properties
*/

/*!
    \fn void addObject(QObject * object)
    \brief Adds \a object to object pool in PluginManager.
*/
void addObject(QObject * object)
{

}

/*!
    \fn void removeObject(QObject * object)
    \brief Removes \a object from object pool in PluginManager.
*/
void removeObject(QObject * object)
{

}

// ============= IPluginPrivate =============

IPluginPrivate::IPluginPrivate()
{
}

IPluginPrivate::~IPluginPrivate()
{
}

