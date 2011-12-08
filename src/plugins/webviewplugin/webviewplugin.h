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

    bool initialize(const QVariantMap &);
    void shutdown();

private:
    void loadSettings();
    void loadAppearanceSettings();

private:
    QWebSettings *m_webSettings;
    QSettings *m_settings;
};

#endif // WEBVIEWPLUGINIMPL_H
