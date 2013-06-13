#include "webvieweditor.h"
#include "webvieweditor_p.h"

#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#if QT_VERSION >= 0x050000
#include <QtCore/QStandardPaths>
#include <QtWidgets/QAction>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebInspector>
#else
#include <IO/QStandardPaths>
#include <QtGui/QAction>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebInspector>
#endif

#include <QtWebKit/QWebHistory>

#include <Parts/EditorWindowFactory>
#include <Parts/constants.h>

#include <Widgets/MiniSplitter>

#include "cookiejar.h"
#include "webviewconstants.h"
#include "webviewdocument.h"
#include "webviewplugin.h"

using namespace Parts;
using namespace WebView;

static inline QString getCacheDirectory()
{
    QString directory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (directory.isEmpty()) {
        directory = QString(QLatin1String("%1/.%2/caches")).
                arg(QDir::homePath()).
                arg(QCoreApplication::applicationName());
    }
    if (!QFile::exists(directory))
        QDir().mkpath(directory);

    return directory;
}

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
    IHistory(parent),
    m_index(-1)
{
}

void WebViewHistory::setHistory(QWebHistory *history)
{
    m_history = history;
    m_index = history->count() ? history->currentItemIndex() : -1;
}

void WebViewHistory::clear()
{
    m_history->clear();
    m_index = 0;
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

    m_index = index;

    m_history->goToItem(m_history->itemAt(index));

    emit currentItemIndexChanged(index);
}

HistoryItem WebViewHistory::itemAt(int index) const
{
    QWebHistoryItem item = m_history->itemAt(index);
    HistoryItem result;
    result.setUrl(item.url());
//    result.setIcon(item.icon());
    result.setLastVisited(item.lastVisited());
    result.setTitle(item.title());
    return result;
}

QByteArray WebViewHistory::store() const
{
    QByteArray history;
    QDataStream s(&history, QIODevice::WriteOnly);
    s << *m_history;
    return history;
}

void WebViewHistory::restore(const QByteArray &arr)
{
    QByteArray history(arr);
    QDataStream s(&history, QIODevice::ReadOnly);
    s >> *m_history;
}

void WebViewHistory::updateCurrentItemIndex()
{
    int index = m_history->currentItemIndex();
    if (m_index == index)
        return;

    m_index = index;

    emit currentItemIndexChanged(index);
}

WebViewEditor::WebViewEditor(QWidget *parent) :
    AbstractEditor(*new WebViewDocument, parent),
    m_webInspector(0)
{
    document()->setParent(this);
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_find = new WebViewFind(this);

    m_splitter = new MiniSplitter(Qt::Vertical, this);
    m_layout->addWidget(m_splitter);

    m_webView = new QWebView(this);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    m_splitter->addWidget(m_webView);

    QWebSettings::setIconDatabasePath(getCacheDirectory());

    connect(m_webView, SIGNAL(linkClicked(QUrl)), SLOT(onUrlClicked(QUrl)));

    createActions();
    connectDocument(qobject_cast<WebViewDocument *>(document()));
}

WebViewEditor::~WebViewEditor()
{
    delete m_webView;
}

void WebViewEditor::setDocument(AbstractDocument *document)
{
    WebViewDocument *webDocument = qobject_cast<WebViewDocument*>(document);
    if (!webDocument)
        return;

    connectDocument(webDocument);

    AbstractEditor::setDocument(document);
}

IFind *WebViewEditor::find() const
{
    return m_find;
}

void WebViewEditor::onUrlClicked(const QUrl &url)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    factory->open(url);
}

void WebViewEditor::showWebInspector(bool show)
{
    if (!m_webInspector) {
        m_webInspector = new QWebInspector(this);
        m_webInspector->setPage(m_webView->page());
        m_splitter->addWidget(m_webInspector);
    }

    m_webInspector->setVisible(show);
}

void WebViewEditor::onPageChanged()
{
    WebViewDocument *document = qobject_cast<WebViewDocument *>(sender());
    Q_ASSERT(document);

    QWebPage *page = document->page();
    m_webView->setPage(page);
    if (m_webInspector)
        m_webInspector->setPage(page);
}

void WebViewEditor::createActions()
{
    m_showWebInspectorAction = new QAction(this);
    m_showWebInspectorAction->setObjectName(Constants::Actions::ShowWebInspector);
    m_showWebInspectorAction->setCheckable(true);
    addAction(m_showWebInspectorAction);
    connect(m_showWebInspectorAction, SIGNAL(triggered(bool)), SLOT(showWebInspector(bool)));
}

void WebViewEditor::connectDocument(WebViewDocument *document)
{
    Q_ASSERT(document);

    QWebPage *page = document->page();
    m_webView->setPage(page);
    connect(document, SIGNAL(pageChanged()), SLOT(onPageChanged()));

    addAction(m_webView->pageAction(QWebPage::Redo));
    addAction(m_webView->pageAction(QWebPage::Undo));

    addAction(page->action(QWebPage::Cut));
    addAction(page->action(QWebPage::Copy));
    addAction(page->action(QWebPage::Paste));
    addAction(page->action(QWebPage::SelectAll));

    connect(WebHistoryInterface::instance(), SIGNAL(itemAdded()),
            document->history(), SLOT(updateCurrentItemIndex()));
}

/*!
    \class WebViewEditorFactory
*/

/*!
    Creates WebViewEditorFactory with the given \a parent.
*/
WebViewEditorFactory::WebViewEditorFactory(QObject *parent) :
    AbstractEditorFactory("WebView", parent)
{
}

/*!
    \reimp
*/
AbstractEditor * WebViewEditorFactory::createEditor(QWidget *parent)
{
    return new WebViewEditor(parent);
}
