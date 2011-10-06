#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QtPlugin>
#include <iplugin.h>

class WebViewTest : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    WebViewTest();

    bool initialize(const QVariantMap &);
    void shutdown();

    QObject * m_view;
};

#endif // TESTPLUGIN_H
