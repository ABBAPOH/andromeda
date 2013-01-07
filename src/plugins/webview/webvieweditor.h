#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webview_global.h"

#include <QtCore/QPointer>
#include <QtGui/QToolBar>
#include <QtWebKit/QWebView>
#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class QVBoxLayout;
class QWebView;
class QWebInspector;
class QSplitter;

namespace WebView {

class WebViewDocument;
class WebViewHistory;
class WebViewFind;

class WEBVIEW_EXPORT WebViewEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditor)

public:
    explicit WebViewEditor(QWidget *parent = 0);
    ~WebViewEditor();

    void setDocument(GuiSystem::AbstractDocument *document);

    GuiSystem::IFind *find() const;
    GuiSystem::IHistory *history() const;

    QWidget *widget() const { return m_webView; }

    QWebView *webView() const { return m_webView; }

private slots:
    void onUrlClicked(const QUrl &url);
    void showWebInspector(bool show);
    void onPageChanged();

private:
    void createActions();
    void connectDocument(WebViewDocument *document);

private:
    QVBoxLayout *m_layout;
    QSplitter *m_splitter;
    QWebView *m_webView;
    WebViewHistory *m_history;
    WebViewFind *m_find;
    QWebInspector *m_webInspector;
    QAction *m_showWebInspectorAction;
};

class WebViewEditorFactory : public GuiSystem::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditorFactory)

public:
    explicit WebViewEditorFactory(QObject * parent = 0) : AbstractEditorFactory(parent) {}
    ~WebViewEditorFactory() {}

    QByteArray id() const { return "WebView"; }
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const { return QStringList() << "text/html"; }
    QStringList urlSchemes() const { return QStringList() << "http" << "https"; }

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace WebView

#endif // WEBVIEWEDITOR_H
