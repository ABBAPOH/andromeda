#include "webviewtest.h"

#include "QWebView"

WebViewTest::WebViewTest()
    : IPlugin()
{
}

bool WebViewTest::initialize()
{
    qDebug("TestPlugin::initialize");
    QWebView * view = new QWebView();
    m_view = view;
    view->show();
    view->load(QUrl("http://ya.ru"));
    return true;
}

void WebViewTest::shutdown()
{
    qDebug("WebViewTest::shutdown");
    delete m_view;
}

QList<ExtensionSystem::PluginDependency> WebViewTest::dependencies()
{
    return QList<ExtensionSystem::PluginDependency>() << ExtensionSystem::PluginDependency("Third Test Plugin", "2.0.1");
}

Q_EXPORT_PLUGIN(WebViewTest)
