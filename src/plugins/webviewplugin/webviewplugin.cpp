#include "webviewplugin.h"

#include <QtCore/QtPlugin>
#include <QtGui/QFileIconProvider>

#include <coreplugin/core.h>
#include <coreplugin/editormanager.h>
#include <coreplugin/settingspagemanager.h>

#include "webvieweditor.h"
#include "webviewsettingspage.h"

using namespace CorePlugin;
using namespace WebViewPlugin;

WebViewPluginImpl::WebViewPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool WebViewPluginImpl::initialize(const QVariantMap &)
{
    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");
    pageManager->addPage(new WebViewSettingsPage);

    Core::instance()->editorManager()->addFactory(new WebViewEditorFactory(this));

    return true;
}

void WebViewPluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
