#ifndef WEBVIEWDOCUMENT_H
#define WEBVIEWDOCUMENT_H

#include "webview_global.h"

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/AbstractDocumentFactory>

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

class WebViewDocumentFactory : public GuiSystem::AbstractDocumentFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewDocumentFactory)

public:
    explicit WebViewDocumentFactory(QObject * parent = 0);

    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;
    QStringList urlSchemes() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
};


} // namespace WebView

#endif // WEBVIEWDOCUMENT_H
