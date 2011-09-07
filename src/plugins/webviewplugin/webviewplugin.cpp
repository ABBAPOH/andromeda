#include "webviewplugin.h"

#include <QtCore/QtPlugin>
#include <core.h>
#include <guicontroller.h>
#include <perspective.h>
#include <perspectivemanager.h>

#include "webvieweditor.h"

using namespace WebViewPlugin;
using namespace GuiSystem;

WebViewPluginImpl::WebViewPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool WebViewPluginImpl::initialize()
{
    GuiController::instance()->addFactory(new WebViewFactory(this));

    Perspective *perspective = new Perspective("WebViewPerspective", this);
    perspective->addView("WebView", 0);
    perspective->setProperty("MainView", "WebView");
    GuiController::instance()->addPerspective(perspective);

    CorePlugin::Core::instance()->perspectiveManager()->addPerspective("text/html",
                                                                       "WebViewPerspective");

    return true;
}

void WebViewPluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
