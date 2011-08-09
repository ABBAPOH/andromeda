#ifndef WEBVIEWEDITOR_H
#define WEBVIEWEDITOR_H

#include "webviewplugin_global.h"

#include <QtWebKit/QWebView>
#include <QtGui/QToolBar>

class QWebView;

namespace WebViewPlugin {

class WEBVIEWPLUGIN_EXPORT WebViewEditor : public QObject
{
    Q_OBJECT
public:
    explicit WebViewEditor(QObject *parent = 0);
    ~WebViewEditor();

    bool open(const QString &path)
    {
        m_webView->setUrl(path);
        return true;
    }
    bool create() { return false; }
    void close() {}

    //    QList<IFile *> files();

    QWidget *widget() const { return m_webView; }
    QToolBar *toolBar() const { return 0; }

    QString name() const { return ""; }

public:
//private:
    QWebView *m_webView;
};

} // namespace WebViewPlugin

#endif // WEBVIEWEDITOR_H
