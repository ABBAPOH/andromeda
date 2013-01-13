#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <ExtensionSystem/IPlugin>

class WebViewTest : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    WebViewTest();

    bool initialize();
    void shutdown();

    QObject * m_view;
};

#endif // TESTPLUGIN_H
