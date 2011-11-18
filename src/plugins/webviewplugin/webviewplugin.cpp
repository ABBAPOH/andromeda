#include "webviewplugin.h"

#include <QtCore/QtPlugin>
#include <QtGui/QFileIconProvider>

#include <guisystem/editormanager.h>
#include <coreplugin/core.h>
#include <coreplugin/settingspagemanager.h>

#include "webvieweditor.h"
#include "webviewsettingspage.h"

using namespace GuiSystem;
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

    EditorManager::instance()->addFactory(new WebViewEditorFactory(this));

    return true;
}

void WebViewPluginImpl::shutdown()
{
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
