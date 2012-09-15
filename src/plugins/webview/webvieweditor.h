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
//class IFind;

namespace GuiSystem {
class FindToolBar;
} // namespace GuiSystem

namespace WebView {

class WebViewHistory;
class WebViewFind;

class WEBVIEW_EXPORT WebViewEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditor)

public:
    explicit WebViewEditor(QWidget *parent = 0);
    ~WebViewEditor();

    void close() {}

    QUrl url() const;

    GuiSystem::IFind *find() const;
    GuiSystem::IHistory *history() const;

    QWidget *widget() const { return m_webView; }

    QWebView *webView() const { return m_webView; }

    QIcon icon() const { return m_webView->icon(); }
    QString title() const { return m_webView->title(); }

public slots:
    void open(const QUrl &url);

    void refresh();
    void cancel();

    void clear();

private slots:
    void onUrlClicked(const QUrl &url);
    void onIconChanged();
    void showWebInspector(bool show);

private:
    void createActions();

private:
    QVBoxLayout *m_layout;
    QSplitter *m_splitter;
    QWebView *m_webView;
    WebViewHistory *m_history;
    WebViewFind *m_find;
    GuiSystem::FindToolBar *m_findToolBar;
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
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace WebView

#endif // WEBVIEWEDITOR_H
