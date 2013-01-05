#ifndef WEBVIEWDOCUMENT_H
#define WEBVIEWDOCUMENT_H

#include "webview_global.h"

#include <guisystem/abstractdocument.h>

class QWebPage;

namespace WebView {

class WEBVIEW_EXPORT WebViewDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit WebViewDocument(QObject *parent = 0);

    QWebPage *page() const;
    void recreatePage();

public slots:
    void stop();
    void clear();
    void reload();

signals:
    void pageChanged();

protected:
    bool openUrl(const QUrl &url);

protected slots:
    void onIconChanged();
    void onTitleChanged(const QString &title);

private:
    QWebPage *m_page;
};

} // namespace WebView

#endif // WEBVIEWDOCUMENT_H
