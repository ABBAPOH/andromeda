#include "webvieweditor.h"

using namespace WebViewPlugin;

WebViewEditor::WebViewEditor(QObject *parent) :
    QObject(parent)
{
    m_webView = new QWebView;
}

WebViewEditor::~WebViewEditor()
{
    qDebug("~WebViewEditor");
    delete m_webView;
}
