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

    connect(m_webView, SIGNAL(titleChanged(QString)), SIGNAL(titleChanged(QString)));
    connect(m_webView, SIGNAL(iconChanged()), SLOT(onIconChanged()));

    connect(m_webView, SIGNAL(loadStarted()), SIGNAL(loadStarted()));
    connect(m_webView, SIGNAL(loadProgress(int)), SIGNAL(loadProgress(int)));
    connect(m_webView, SIGNAL(loadFinished(bool)), SIGNAL(loadFinished(bool)));
}

WebViewEditor::~WebViewEditor()
{
    delete m_webView;
}

bool WebViewEditor::open(const QUrl &url)
{
    m_webView->load(url);
    emit currentUrlChanged(url);
    return true;
}

QUrl WebViewPlugin::WebViewEditor::currentUrl() const
{
    return m_webView->url();
}

void WebViewEditor::refresh()
{
    m_webView->reload();
}

void WebViewEditor::cancel()
{
    m_webView->stop();
}

void WebViewEditor::resizeEvent(QResizeEvent *e)
{
    m_webView->resize(e->size());
}

void WebViewEditor::onUrlClicked(const QUrl &url)
{
    m_webView->load(url);
    emit currentUrlChanged(url);
}

void WebViewEditor::onIconChanged()
{
    emit iconChanged(m_webView->icon());
}

CorePlugin::AbstractEditor * WebViewPlugin::WebViewEditorFactory::createEditor(QWidget *parent)
{
    return new WebViewEditor(parent);
}
