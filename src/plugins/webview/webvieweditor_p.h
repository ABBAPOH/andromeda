#ifndef WEBVIEWEDITOR_P_H
#define WEBVIEWEDITOR_P_H

#include "webvieweditor.h"

#include <guisystem/abstracthistory.h>
#include <guisystem/ifind.h>

namespace WebView {

class WebViewFind : public GuiSystem::IFind
{
public:
    explicit WebViewFind(WebViewEditor *editor);

    FindFlags supportedFindFlags() const;
    void resetIncrementalSearch();
    void clearResults();

    QString currentFindString() const;
    QString completedFindString() const;

    void highlightAll(const QString &txt, FindFlags findFlags);
    void findIncremental(const QString &txt, FindFlags findFlags);
    void findStep(const QString &txt, FindFlags findFlags);

private:
    WebViewEditor *m_editor;

    QString m_currentFindString;
};

class WebViewHistory : public GuiSystem::AbstractHistory
{
    Q_OBJECT
    Q_DISABLE_COPY(WebViewHistory)

public:
    explicit WebViewHistory(QObject *parent = 0);

    inline void setHistory(QWebHistory *history) { m_history = history; }

    virtual void clear();
    virtual int count() const;

    virtual int currentItemIndex() const;
    virtual void setCurrentItemIndex(int index);

    virtual GuiSystem::HistoryItem itemAt(int index) const;

private:
    QWebHistory *m_history;
};

} //namespace WebView

#endif // WEBVIEWEDITOR_P_H
