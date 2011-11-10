#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webviewplugin_global.h"

#include <QtCore/QPointer>
#include <QtGui/QToolBar>
#include <QtWebKit/QWebView>
#include <abstracteditor.h>
#include <abstracteditorfactory.h>
#include <historyitem.h>

class QWebView;

namespace WebViewPlugin {

class WEBVIEWPLUGIN_EXPORT WebViewEditor : public CorePlugin::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditor)

public:
    explicit WebViewEditor(QWidget *parent = 0);
    ~WebViewEditor();

    bool create() { return false; }
    void close() {}

    bool open(const QUrl &url);

    QUrl currentUrl() const;

    void refresh();
    void cancel();

    QWidget *widget() const { return m_webView; }
    QToolBar *toolBar() const { return 0; }

    QIcon icon() const { return m_webView->icon(); }
    QString title() const { return m_webView->title(); }
    QString windowTitle() const { return m_webView->windowTitle(); }

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onUrlClicked(const QUrl &url);
    void onIconChanged();

private:
    QWebView *m_webView;
};

class WebViewEditorFactory : public CorePlugin::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditorFactory)

public:
    explicit WebViewEditorFactory(QObject * parent = 0) : CorePlugin::AbstractEditorFactory(parent) {}
    ~WebViewEditorFactory() {}

    QByteArray id() const { return "WebView"; }
    QStringList mimeTypes() { return QStringList() << "text/html"; }

protected:
    CorePlugin::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
