#ifndef WEBVIEWPLUGINIMPL_H
#define WEBVIEWPLUGINIMPL_H

#include <extensionsystem/iplugin.h>

class WebViewPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    WebViewPluginImpl();

    bool initialize(const QVariantMap &);
    void shutdown();
};

#endif // WEBVIEWPLUGINIMPL_H
