#include "webviewtest.h"

#include "QWebView"

WebViewTest::WebViewTest()
    : IPlugin()
{
}

bool WebViewTest::initialize()
{
    qDebug("WebViewTest::initialize");
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

Q_EXPORT_PLUGIN(WebViewTest)
