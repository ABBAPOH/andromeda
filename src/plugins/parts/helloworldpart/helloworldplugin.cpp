#include "helloworldplugin.h"

#include <QtCore/QtPlugin>
#include <Parts/DocumentManager>
#include <Parts/EditorManager>

#include "helloworlddocument.h"
#include "helloworldeditor.h"

using namespace Parts;
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
    DocumentManager::instance()->addFactory(new HelloWorldDocumentFactory(this));
    EditorManager::instance()->addFactory(new HelloWorldEditorFactory(this));

    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(HelloWorldPlugin)
#endif
