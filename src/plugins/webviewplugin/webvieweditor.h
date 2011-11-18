#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webviewplugin_global.h"

#include <QtCore/QPointer>
#include <QtGui/QToolBar>
#include <QtWebKit/QWebView>
#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class QWebView;

namespace WebViewPlugin {

class WebViewHistory;

class WEBVIEWPLUGIN_EXPORT WebViewEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditor)

public:
    explicit WebViewEditor(QWidget *parent = 0);
    ~WebViewEditor();

    Capabilities capabilities() const;

    void close() {}

    QUrl url() const;

    GuiSystem::AbstractHistory *history() const;

    QWidget *widget() const { return m_webView; }
    QToolBar *toolBar() const { return 0; }

    QIcon icon() const { return m_webView->icon(); }
    QString title() const { return m_webView->title(); }
    QString windowTitle() const { return m_webView->windowTitle(); }

public slots:
    void open(const QUrl &url);

    void refresh();
    void cancel();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onUrlClicked(const QUrl &url);
    void onIconChanged();

private:
    QWebView *m_webView;
    WebViewHistory *m_history;
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

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
