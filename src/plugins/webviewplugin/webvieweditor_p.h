#ifndef WEBVIEWEDITOR_P_H
#define WEBVIEWEDITOR_P_H

#include "webvieweditor.h"

#include <guisystem/abstracthistory.h>

namespace WebViewPlugin {

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

} //namespace WebViewPlugin

#endif // WEBVIEWEDITOR_P_H
