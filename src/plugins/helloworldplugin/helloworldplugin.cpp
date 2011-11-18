#include "helloworldplugin.h"
#include "helloworldeditor.h"

#include <QtCore/QtPlugin>
#include <guisystem/editormanager.h>

using namespace GuiSystem;

HelloWorlPluginImpl::HelloWorlPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool HelloWorlPluginImpl::initialize(const QVariantMap &)
{
    HelloWorldEditorFactory *f = new HelloWorldEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(HelloWorlPluginImpl)
