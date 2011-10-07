#include "webvieweditor.h"

#include <QtGui/QResizeEvent>

using namespace WebViewPlugin;
using namespace CorePlugin;

WebViewEditor::WebViewEditor(QWidget *parent) :
    AbstractEditor(parent)
{
    m_webView = new QWebView(this);
    m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_webView, SIGNAL(linkClicked(QUrl)), SLOT(onUrlClicked(QUrl)));

    connect(m_webView, SIGNAL(titleChanged(QString)), SIGNAL(changed()));
    connect(m_webView, SIGNAL(iconChanged()), SIGNAL(changed()));
}

WebViewEditor::~WebViewEditor()
{
    delete m_webView;
}

bool WebViewEditor::open(const QUrl &url)
{
    m_webView->setUrl(url);
    return true;
}

QUrl WebViewPlugin::WebViewEditor::currentUrl() const
{
    return m_webView->url();
}

void WebViewEditor::resizeEvent(QResizeEvent *e)
{
    m_webView->resize(e->size());
}

void WebViewEditor::onUrlClicked(const QUrl &url)
{
    m_webView->setUrl(url);
    emit currentUrlChanged(url);
}

CorePlugin::AbstractEditor * WebViewPlugin::WebViewEditorFactory::createEditor(QWidget *parent)
{
    return new WebViewEditor(parent);
}
