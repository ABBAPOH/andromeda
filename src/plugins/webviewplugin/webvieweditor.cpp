#include "webvieweditor.h"

using namespace WebViewPlugin;
using namespace Core;

WebViewEditor::WebViewEditor(QObject *parent) :
    IEditor(parent)
{
    m_webView = new QWebView;
}
