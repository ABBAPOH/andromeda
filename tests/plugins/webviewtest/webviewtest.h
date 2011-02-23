#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <iplugin.h>

class WebViewTest : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    WebViewTest();

    virtual bool initialize();
    virtual void shutdown();

    QObject * m_view;
};

#endif // TESTPLUGIN_H
