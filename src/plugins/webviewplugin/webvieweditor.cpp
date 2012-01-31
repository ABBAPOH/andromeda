#include "webvieweditor.h"
#include "webvieweditor_p.h"

#include <QtCore/QFile>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebHistory>

#include <guisystem/findtoolbar.h>
#include <coreplugin/constants.h>

#include "cookiejar.h"
#include "webviewplugin.h"

using namespace GuiSystem;
using namespace WebViewPlugin;

WebViewFind::WebViewFind(WebViewEditor *editor) :
    IFind(editor),
    m_editor(editor)
{
}

IFind::FindFlags WebViewFind::supportedFindFlags() const
{
    return IFind::FindFlags(IFind::FindBackward | IFind::FindCaseSensitively);
}

void WebViewFind::resetIncrementalSearch()
{
    m_currentFindString.clear();
    m_editor->webView()->findText(QString(), QWebPage::HighlightAllOccurrences);
}

void WebViewFind::clearResults()
{
    resetIncrementalSearch();
}

QString WebViewFind::currentFindString() const
{
    return m_currentFindString;
}

QString WebViewFind::completedFindString() const
{
    return m_currentFindString;
}

void WebViewFind::highlightAll(const QString &txt, IFind::FindFlags findFlags)
{
    QWebPage::FindFlags flags = QWebPage::HighlightAllOccurrences;

    if (findFlags & IFind::FindCaseSensitively)
        flags |= QWebPage::FindCaseSensitively;

    m_editor->webView()->findText(QString(), flags);
    m_editor->webView()->findText(txt, flags);
}

void WebViewFind::findIncremental(const QString &txt, IFind::FindFlags findFlags)
{
    QWebPage::FindFlags flags;

    if (findFlags & IFind::FindCaseSensitively)
        flags |= QWebPage::FindCaseSensitively;

    m_currentFindString = txt;
    m_editor->webView()->findText(txt, flags);
}

void WebViewFind::findStep(const QString &txt, IFind::FindFlags findFlags)
{
    QWebPage::FindFlags flags = QWebPage::FindWrapsAroundDocument;

    if (findFlags & IFind::FindBackward)
        flags |= QWebPage::FindBackward;

    if (findFlags & IFind::FindCaseSensitively)
        flags |= QWebPage::FindCaseSensitively;

    m_currentFindString = txt;
    m_editor->webView()->findText(txt, flags);
}

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
    if (m_history->count() == 0)
        return;

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
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_find = new WebViewFind(this);

    m_findToolBar = new FindToolBar(this);
    m_findToolBar->setFind(m_find);
    m_findToolBar->hide();
    m_layout->addWidget(m_findToolBar);

    CookieJar *jar = WebViewPluginImpl::instance()->cookieJar();

    m_webView = new QWebView(this);
    m_webView->page()->networkAccessManager()->setCookieJar(jar);
    jar->setParent(WebViewPluginImpl::instance());
    m_layout->addWidget(m_webView);

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

    QAction *findAction = new QAction(tr("Find"), this);
    connect(findAction, SIGNAL(triggered()), m_findToolBar, SLOT(openFind()));
    addAction(findAction, "Actions.Find");

    addAction(m_webView->pageAction(QWebPage::Redo), Constants::Actions::Redo);
    addAction(m_webView->pageAction(QWebPage::Undo), Constants::Actions::Undo);

    addAction(m_webView->pageAction(QWebPage::Cut), Constants::Actions::Cut);
    addAction(m_webView->pageAction(QWebPage::Copy), Constants::Actions::Copy);
    addAction(m_webView->pageAction(QWebPage::Paste), Constants::Actions::Paste);
    addAction(m_webView->pageAction(QWebPage::SelectAll), Constants::Actions::SelectAll);
}

WebViewEditor::~WebViewEditor()
{
    delete m_webView;
}

AbstractEditor::Capabilities WebViewEditor::capabilities() const
{
    return AbstractEditor::Capabilities(AbstractEditor::HasHistory |
                                        AbstractEditor::HasFind |
                                        AbstractEditor::CanSave);
}

void WebViewEditor::open(const QUrl &url)
{
    m_webView->load(url);
}

QUrl WebViewEditor::url() const
{
    return m_webView->url();
}

IFind *WebViewEditor::find() const
{
    return m_find;
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

void WebViewEditor::save(const QUrl &url)
{
    QString html = m_webView->page()->mainFrame()->toHtml();
    if (url.scheme() == QLatin1String("file")) {
        QFile file(url.path());
        if (!file.open(QFile::WriteOnly | QFile::Text))
            return;

        file.write(html.toUtf8());
    }
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
