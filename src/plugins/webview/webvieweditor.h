#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webview_global.h"

#include <QtCore/QPointer>
#include <QtGui/QToolBar>
#include <QtWebKit/QWebView>
#include <GuiSystem/AbstractEditor>
#include <GuiSystem/AbstractEditorFactory>

class QVBoxLayout;
class QWebView;
class QWebInspector;
class QSplitter;

namespace WebView {

class WebViewDocument;
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
    WebViewFind *m_find;
    QWebInspector *m_webInspector;
    QAction *m_showWebInspectorAction;
};

class WebViewEditorFactory : public GuiSystem::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditorFactory)

public:
    explicit WebViewEditorFactory(QObject * parent = 0);

    QByteArray id() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace WebView

#endif // WEBVIEWEDITOR_H
