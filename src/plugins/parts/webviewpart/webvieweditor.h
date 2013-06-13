#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include <QtCore/QPointer>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QToolBar>
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#include <QtGui/QToolBar>
#endif

#include <Parts/AbstractEditor>
#include <Parts/AbstractEditorFactory>

class QVBoxLayout;
class QWebView;
class QWebInspector;
class QSplitter;

namespace WebView {

class WebViewDocument;
class WebViewFind;

class WebViewEditor : public Parts::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditor)

public:
    explicit WebViewEditor(QWidget *parent = 0);
    ~WebViewEditor();

    void setDocument(Parts::AbstractDocument *document);

    Parts::IFind *find() const;

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

class WebViewEditorFactory : public Parts::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditorFactory)

public:
    explicit WebViewEditorFactory(QObject * parent = 0);

protected:
    Parts::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace WebView

#endif // WEBVIEWEDITOR_H
