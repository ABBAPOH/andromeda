#include "helloworldplugin.h"
#include "helloworldeditor.h"

#include <QtCore/QtPlugin>
#include <guisystem/editormanager.h>

using namespace GuiSystem;
using namespace HelloWorld;

/*!
    \class HelloWorld::HelloWorldPlugin

    HelloWorldPlugin is an ExtensionSystem::IPlugin interface implementaion.
*/

/*!
    Creates HelloWorldPlugin.
*/
HelloWorldPlugin::HelloWorldPlugin() :
    ExtensionSystem::IPlugin()
{
}

/*!
    Destorys HelloWorldPlugin.
*/
HelloWorldPlugin::~HelloWorldPlugin()
{
}

/*!
    \reimp
*/
bool HelloWorldPlugin::initialize()
{
    HelloWorldEditorFactory *f = new HelloWorldEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(HelloWorldPlugin)
