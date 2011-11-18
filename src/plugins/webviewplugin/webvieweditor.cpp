#include "webvieweditor.h"
#include "webvieweditor_p.h"

#include <QtGui/QResizeEvent>
#include <QtWebKit/QWebHistory>

using namespace GuiSystem;
using namespace WebViewPlugin;

WebViewHistory::WebViewHistory(QObject *parent) :
    AbstractHistory(parent)
{
}

void WebViewHistory::clear()
{
    m_history->clear();
}

int WebViewHistory::count() const
{
    return m_history->count();
}

int WebViewHistory::currentItemIndex() const
{
    return m_history->currentItemIndex();
}

void WebViewHistory::setCurrentItemIndex(int index)
{
    m_history->goToItem(m_history->itemAt(index));
}

HistoryItem WebViewHistory::itemAt(int index) const
{
    QWebHistoryItem item = m_history->itemAt(index);
    HistoryItem result;
    result.setPath(item.url().toString());
    result.setIcon(item.icon());
    result.setLastVisited(item.lastVisited());
    result.setTitle(item.title());
    return result;
}

WebViewEditor::WebViewEditor(QWidget *parent) :
    AbstractEditor(parent)
{
    m_webView = new QWebView(this);
    m_history = new WebViewHistory(this);
    m_history->setHistory(m_webView->history());
    m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_webView, SIGNAL(urlChanged(QUrl)), SIGNAL(urlChanged(QUrl)));
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

AbstractEditor::Capabilities WebViewEditor::capabilities() const
{
    return AbstractEditor::HasHistory;
}

void WebViewEditor::open(const QUrl &url)
{
    m_webView->load(url);
}

QUrl WebViewEditor::url() const
{
    return m_webView->url();
}

AbstractHistory *WebViewEditor::history() const
{
    return m_history;
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
//    emit urlChanged(url);
}

void WebViewEditor::onIconChanged()
{
    emit iconChanged(m_webView->icon());
}

AbstractEditor * WebViewEditorFactory::createEditor(QWidget *parent)
{
    return new WebViewEditor(parent);
}
