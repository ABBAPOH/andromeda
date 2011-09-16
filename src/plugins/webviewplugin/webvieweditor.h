#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webviewplugin_global.h"

#include <QtCore/QPointer>
#include <QtGui/QToolBar>
#include <QtWebKit/QWebView>
#include <ieditor.h>
#include <iviewfactory.h>
#include <historyitem.h>

class QWebView;

namespace WebViewPlugin {

class WEBVIEWPLUGIN_EXPORT WebViewEditor : public CorePlugin::IEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewEditor)

public:
    explicit WebViewEditor(QObject *parent = 0);
    ~WebViewEditor();

    bool create() { return false; }
    void close() {}

    bool open(const QString &path);

    QString currentPath() const;

    QWidget *widget() const { return m_webView; }
    QToolBar *toolBar() const { return 0; }

    QIcon icon() const { return m_webView->icon(); }
    QString title() const { return m_webView->title(); }
    QString windowTitle() const { return m_webView->windowTitle(); }

private slots:
    void onUrlClicked(const QUrl &url);

private:
    QPointer<QWebView> m_webView;
};

class WebViewFactory : public GuiSystem::IViewFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewFactory)

public:
    explicit WebViewFactory(QObject * parent = 0) : GuiSystem::IViewFactory(parent) {}
    ~WebViewFactory() {}

    QString id() const { return "WebView"; }
    QString type() const { return "WebView"; }

protected:
    GuiSystem::IView *createView();
};

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
