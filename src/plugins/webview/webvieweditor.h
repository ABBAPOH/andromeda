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

    Capabilities capabilities() const;

    void close() {}

    QUrl url() const;

    GuiSystem::IFind *find() const;
    GuiSystem::AbstractHistory *history() const;

    QWidget *widget() const { return m_webView; }

    QWebView *webView() const { return m_webView; }

    QIcon icon() const { return m_webView->icon(); }
    QString title() const { return m_webView->title(); }
    QString windowTitle() const { return m_webView->windowTitle(); }

public slots:
    void open(const QUrl &url);

    void refresh();
    void cancel();

    void save(const QUrl &url = QUrl());

private slots:
    void onUrlClicked(const QUrl &url);
    void onIconChanged();

private:
    QVBoxLayout *m_layout;
    QWebView *m_webView;
    WebViewHistory *m_history;
    WebViewFind *m_find;
    GuiSystem::FindToolBar *m_findToolBar;
};

class WebViewEditorFactory : public GuiSystem::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditorFactory)

public:
    explicit WebViewEditorFactory(QObject * parent = 0) : AbstractEditorFactory(parent) {}
    ~WebViewEditorFactory() {}

    QByteArray id() const { return "WebView"; }
    QStringList mimeTypes() { return QStringList() << "text/html"; }

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace WebView

#endif // WEBVIEWEDITOR_H
