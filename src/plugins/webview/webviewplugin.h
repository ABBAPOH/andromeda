#ifndef WEBVIEWPLUGINIMPL_H
#define WEBVIEWPLUGINIMPL_H

#include <extensionsystem/iplugin.h>

class CookieJar;
class QSettings;
class QWebSettings;

class WebViewPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    WebViewPlugin();
    ~WebViewPlugin();

    static WebViewPlugin *instance();

    inline CookieJar *cookieJar() const { return m_cookieJar; }

    bool initialize(const QVariantMap &);
    void shutdown();

private:
    void loadSettings();
    void loadAppearanceSettings();
    void loadProxySettings();

private:
    CookieJar *m_cookieJar;
    QWebSettings *m_webSettings;
    QSettings *m_settings;

    friend class ProxySettingsWidget;
};

#endif // WEBVIEWPLUGINIMPL_H
