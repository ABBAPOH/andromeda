#include "webviewplugin.h"

#include <QtCore/QtPlugin>

#include "webvieweditor.h"

using namespace WebViewPlugin;

WebViewPluginImpl::WebViewPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool WebViewPluginImpl::initialize()
{
    return true;
}

void WebViewPluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
