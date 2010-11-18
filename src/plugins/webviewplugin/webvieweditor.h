#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webviewplugin_global.h"

#include <ieditor.h>

#include <QtWebKit/QWebView>

class QWebView;

namespace WebViewPlugin {

    class WEBVIEWPLUGIN_EXPORT WebViewEditor : public Core::IEditor
{
    Q_OBJECT
public:
    explicit WebViewEditor(QObject *parent = 0);

    virtual bool open(const QUrl &url)
    {
        m_webView->setUrl(url);
    }
    virtual bool create() { return false; }

    virtual IFile *file() { return 0; }
//    QList<IFile *> files();

    virtual QWidget *widget() { return m_webView; }
    virtual QToolBar *toolBar() { return 0; }

    virtual QString name() { return ""; }

signals:

public slots:

private:
    QWebView *m_webView;
};

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
