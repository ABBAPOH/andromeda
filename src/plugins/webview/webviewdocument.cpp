#include "webviewdocument.h"

#include <QWebFrame>
#include <QWebPage>

using namespace GuiSystem;
using namespace WebView;

static QWebPage * createPage(WebViewDocument *document)
{
    QWebPage *page = new QWebPage(document);

    QObject::connect(page->mainFrame(), SIGNAL(urlChanged(QUrl)),
                     document, SLOT(setUrl(QUrl)));
    QObject::connect(page->mainFrame(), SIGNAL(iconChanged()),
                     document, SLOT(onIconChanged()));
    QObject::connect(page->mainFrame(), SIGNAL(titleChanged(QString)),
                     document, SLOT(onTitleChanged(QString)));

    return page;
}

WebViewDocument::WebViewDocument(QObject *parent) :
    AbstractDocument(parent),
    m_page(createPage(this))
{
}

QWebPage * WebViewDocument::page() const
{
    return m_page;
}

void WebViewDocument::recreatePage()
{
    QWebPage *page = m_page;
    m_page = createPage(this);
    page->deleteLater();
    emit pageChanged();
}

void WebViewDocument::stop()
{
    m_page->triggerAction(QWebPage::Stop);
}

void WebViewDocument::clear()
{
    recreatePage();
}

void WebViewDocument::reload()
{
    m_page->triggerAction(QWebPage::Reload);
}

bool WebViewDocument::openUrl(const QUrl &url)
{
    m_page->mainFrame()->load(url);
    return true;
}

void WebViewDocument::onIconChanged()
{
    setIcon(m_page->mainFrame()->icon());
}

void WebViewDocument::onTitleChanged(const QString &title)
{
    setTitle(title);
}
