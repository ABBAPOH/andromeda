#include "webviewplugin.h"

#include <QtCore/QtPlugin>
#include <QtGui/QFileIconProvider>
#include <core.h>
#include <guicontroller.h>
#include <perspective.h>
#include <perspectivemanager.h>
#include <settingspagemanager.h>

#include "webvieweditor.h"
#include "webviewsettingspage.h"

using namespace GuiSystem;
using namespace CorePlugin;
using namespace WebViewPlugin;

WebViewPluginImpl::WebViewPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool WebViewPluginImpl::initialize()
{
    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");
    pageManager->addPage(new WebViewSettingsPage);

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
