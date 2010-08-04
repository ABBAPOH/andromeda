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

