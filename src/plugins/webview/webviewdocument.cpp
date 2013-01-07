#include "webviewdocument.h"

#include <QtGui/QAction>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>
#include <guisystem/constants.h>

#include "cookiejar.h"
#include "webvieweditor_p.h"
#include "webviewplugin.h"

using namespace GuiSystem;
using namespace WebView;

WebViewDocument::WebViewDocument(QObject *parent) :
    AbstractDocument(parent),
    m_history(new WebViewHistory(this)),
    m_page(createPage(this))
{
}

IHistory * WebViewDocument::history() const
{
    return m_history;
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

QWebPage * WebViewDocument::createPage(WebViewDocument *document)
{
    WebViewPlugin *plugin = WebViewPlugin::instance();
    CookieJar *jar = plugin->cookieJar();
    QWebPage *page = new QWebPage(document);
    page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    page->networkAccessManager()->setCookieJar(jar);
    jar->setParent(plugin); // QNAM reparents jar, fix it here

    QObject::connect(page->mainFrame(), SIGNAL(urlChanged(QUrl)),
                     document, SLOT(setUrl(QUrl)));
    QObject::connect(page->mainFrame(), SIGNAL(iconChanged()),
                     document, SLOT(onIconChanged()));
    QObject::connect(page->mainFrame(), SIGNAL(titleChanged(QString)),
                     document, SLOT(onTitleChanged(QString)));

    QObject::connect(page, SIGNAL(loadStarted()), document, SLOT(onLoadStarted()));
    QObject::connect(page, SIGNAL(loadProgress(int)), document, SLOT(setProgress(int)));
    QObject::connect(page, SIGNAL(loadFinished(bool)), document, SLOT(onLoadFinished()));

    page->action(QWebPage::Redo)->setObjectName(Constants::Actions::Redo);
    page->action(QWebPage::Undo)->setObjectName(Constants::Actions::Undo);

    page->action(QWebPage::Cut)->setObjectName(Constants::Actions::Cut);
    page->action(QWebPage::Copy)->setObjectName(Constants::Actions::Copy);
    page->action(QWebPage::Paste)->setObjectName(Constants::Actions::Paste);
    page->action(QWebPage::SelectAll)->setObjectName(Constants::Actions::SelectAll);

    document->m_history->setHistory(page->history());

    return page;
}

void WebViewDocument::onIconChanged()
{
    setIcon(m_page->mainFrame()->icon());
}

void WebViewDocument::onTitleChanged(const QString &title)
{
    setTitle(title);
}

void WebViewDocument::onLoadStarted()
{
    setState(OpenningState);
}

void WebViewDocument::onLoadFinished()
{
    setState(NoState);
}
