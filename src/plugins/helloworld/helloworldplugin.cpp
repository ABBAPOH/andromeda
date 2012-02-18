#include "helloworldplugin.h"
#include "helloworldeditor.h"

#include <QtCore/QtPlugin>
#include <guisystem/editormanager.h>

using namespace GuiSystem;
using namespace HelloWorld;

HelloWorldPlugin::HelloWorldPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool HelloWorldPlugin::initialize(const QVariantMap &)
{
    HelloWorldEditorFactory *f = new HelloWorldEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(HelloWorldPlugin)
