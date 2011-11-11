#include "helloworldplugin.h"
#include "helloworldeditor.h"

#include <QtCore/QtPlugin>
#include <coreplugin/core.h>
#include <coreplugin/editormanager.h>

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
