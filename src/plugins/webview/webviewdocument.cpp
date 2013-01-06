#include "webviewdocument.h"

#include <QWebFrame>
#include <QWebPage>

#include "cookiejar.h"
#include "webviewplugin.h"

using namespace GuiSystem;
using namespace WebView;

static QWebPage * createPage(WebViewDocument *document)
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

    QObject::connect(page, SIGNAL(loadStarted()), document, SIGNAL(loadStarted()));
    QObject::connect(page, SIGNAL(loadProgress(int)), document, SIGNAL(loadProgress(int)));
    QObject::connect(page, SIGNAL(loadFinished(bool)), document, SIGNAL(loadFinished(bool)));


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
