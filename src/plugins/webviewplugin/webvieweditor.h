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

    virtual bool open(const QString &path)
    {
        m_webView->setUrl(path);
    }
    virtual bool create() { return false; }

    virtual IFile *file() const { return 0; }
//    QList<IFile *> files();

    virtual QWidget *widget() const { return m_webView; }
    virtual QToolBar *toolBar() const { return 0; }

    virtual QString name() const { return ""; }

signals:

public slots:

private:
    QWebView *m_webView;
};

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
