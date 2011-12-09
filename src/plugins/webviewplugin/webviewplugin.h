#ifndef WEBVIEWPLUGINIMPL_H
#define WEBVIEWPLUGINIMPL_H

#include <extensionsystem/iplugin.h>

class QSettings;
class QWebSettings;

class WebViewPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    WebViewPluginImpl();
    ~WebViewPluginImpl();

    static WebViewPluginImpl *instance();

    bool initialize(const QVariantMap &);
    void shutdown();

private:
    void loadSettings();
    void loadAppearanceSettings();
    void loadProxySettings();

private:
    QWebSettings *m_webSettings;
    QSettings *m_settings;

    friend class ProxySettingsWidget;
};

#endif // WEBVIEWPLUGINIMPL_H
