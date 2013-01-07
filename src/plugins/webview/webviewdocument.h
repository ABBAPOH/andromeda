#ifndef WEBVIEWDOCUMENT_H
#define WEBVIEWDOCUMENT_H

#include "webview_global.h"

#include <guisystem/abstractdocument.h>

class QWebPage;

namespace WebView {

class WebViewHistory;

class WEBVIEW_EXPORT WebViewDocument : public GuiSystem::AbstractDocument
{
    Q_OBJECT
public:
    explicit WebViewDocument(QObject *parent = 0);

    GuiSystem::IHistory *history() const;

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
    QWebPage *createPage(WebViewDocument *document);

protected slots:
    void onIconChanged();
    void onTitleChanged(const QString &title);
    void onLoadStarted();
    void onLoadFinished();

private:
    WebViewHistory *m_history;
    QWebPage *m_page;
};

} // namespace WebView

#endif // WEBVIEWDOCUMENT_H
