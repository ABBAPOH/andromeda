#include "helloworldplugin.h"
#include "helloworldeditor.h"

#include <QtCore/QtPlugin>
#include <core.h>
#include <editormanager.h>

HelloWorlPluginImpl::HelloWorlPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool HelloWorlPluginImpl::initialize(const QVariantMap &)
{
    HelloWorldEditorFactory *f = new HelloWorldEditorFactory(this);
    CorePlugin::Core::instance()->editorManager()->addFactory(f);

    return true;
}

Q_EXPORT_PLUGIN(HelloWorlPluginImpl)
