#include "webviewplugin.h"

#include <QtCore/QtPlugin>
#include <QtGui/QFileIconProvider>

#include <guisystem/editormanager.h>
#include <coreplugin/core.h>
#include <coreplugin/settingspagemanager.h>

#include "appearancesettings.h"
#include "webvieweditor.h"
#include "webviewsettingspage.h"

#include <QtCore/QSettings>
#include <QtCore/QVariant>
#include <QtWebKit/QWebSettings>

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
    pageManager->addPage(new AppearanceSettingsPage);

    EditorManager::instance()->addFactory(new WebViewEditorFactory(this));

    loadSettings();

    return true;
}

void WebViewPluginImpl::shutdown()
{
}

void WebViewPluginImpl::loadSettings()
{
    m_settings = new QSettings(this);
    m_webSettings = QWebSettings::globalSettings();

    m_settings->beginGroup(QLatin1String("webview"));
    loadAppearanceSettings();
}

void WebViewPluginImpl::loadAppearanceSettings()
{
    m_settings->beginGroup(QLatin1String("appearance"));

    QString fixedFontFamily = m_webSettings->fontFamily(QWebSettings::FixedFont);
    int fixedFontSize = m_webSettings->fontSize(QWebSettings::DefaultFixedFontSize);

    QString standardFontFamily = m_webSettings->fontFamily(QWebSettings::StandardFont);
    int standardFontSize = m_webSettings->fontSize(QWebSettings::DefaultFontSize);

    QFont fixedFont = QFont(fixedFontFamily, fixedFontSize);
    QFont standardFont = QFont(standardFontFamily, standardFontSize);

    int minimumFontSize = m_webSettings->fontSize(QWebSettings::MinimumFontSize);
    QByteArray defaultEncoding;

    fixedFont = qVariantValue<QFont>(m_settings->value(QLatin1String("fixedFont"), fixedFont));
    standardFont = qVariantValue<QFont>(m_settings->value(QLatin1String("standardFont"), standardFont));
    minimumFontSize = m_settings->value(QLatin1String("minimumFontSize"), minimumFontSize).toInt();
    defaultEncoding = m_settings->value(QLatin1String("defaultEncoding"), "UTF-8").toByteArray();

    m_webSettings->setFontFamily(QWebSettings::FixedFont, fixedFont.family());
    m_webSettings->setFontSize(QWebSettings::DefaultFixedFontSize, fixedFont.pointSize());

    m_webSettings->setFontFamily(QWebSettings::StandardFont, standardFont.family());
    m_webSettings->setFontSize(QWebSettings::DefaultFontSize, standardFont.pointSize());

    if (minimumFontSize)
        m_webSettings->setFontSize(QWebSettings::MinimumFontSize, minimumFontSize);

    m_webSettings->setDefaultTextEncoding(defaultEncoding);

    m_settings->endGroup();
}

Q_EXPORT_PLUGIN(WebViewPluginImpl)
