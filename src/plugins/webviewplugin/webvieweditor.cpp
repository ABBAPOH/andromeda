#include "webvieweditor.h"

using namespace WebViewPlugin;
using namespace CorePlugin;

WebViewEditor::WebViewEditor(QObject *parent) :
    IEditor(parent)
{
    m_webView = new QWebView;
    m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_webView, SIGNAL(linkClicked(QUrl)), SLOT(onUrlClicked(QUrl)));

    connect(m_webView, SIGNAL(titleChanged(QString)), SIGNAL(changed()));
    connect(m_webView, SIGNAL(iconChanged()), SIGNAL(changed()));
}

WebViewEditor::~WebViewEditor()
{
    delete m_webView;
}

bool WebViewEditor::open(const QString &path)
{
    m_webView->setUrl(path);
    return true;
}

QString WebViewPlugin::WebViewEditor::currentPath() const
{
    return m_webView->url().toString();
}

void WebViewEditor::onUrlClicked(const QUrl &url)
{
    m_webView->setUrl(url);
    emit pathChanged(url.toString());
}

GuiSystem::IView * WebViewPlugin::WebViewFactory::createView()
{
    return new WebViewEditor(this);
}
