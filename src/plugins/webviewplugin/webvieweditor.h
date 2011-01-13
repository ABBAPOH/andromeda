#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webviewplugin_global.h"

#include <ieditor.h>

#include <QtWebKit/QWebView>

class QWebView;

namespace WebViewPlugin {

    class WEBVIEWPLUGIN_EXPORT WebViewEditor : public CorePlugin::IEditor
{
    Q_OBJECT
public:
    explicit WebViewEditor(QObject *parent = 0);
    ~WebViewEditor();

    virtual bool open(const QString &path)
    {
        m_webView->setUrl(path);
        return true;
    }
    virtual bool create() { return false; }
    virtual void close() {}

    virtual CorePlugin::IFile *file() const { return m_file; }
//    QList<IFile *> files();

    virtual QWidget *widget() const { return m_webView; }
    virtual QToolBar *toolBar() const { return 0; }

    virtual QString name() const { return ""; }

signals:

public slots:

public:
//private:
    QWebView *m_webView;
    CorePlugin::IFile *m_file;
};

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
