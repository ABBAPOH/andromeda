#ifndef PROXYHISTORY_H
#define PROXYHISTORY_H

#include "abstracthistory.h"

namespace GuiSystem {

class GUISYSTEM_EXPORT ProxyHistory : public AbstractHistory
{
    Q_OBJECT
    Q_DISABLE_COPY(ProxyHistory)

public:
    explicit ProxyHistory(QObject *parent = 0);

    int count() const;

    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    HistoryItem itemAt(int index) const;

    AbstractHistory *sourceHistory() const;
    void setSourceHistory(AbstractHistory *history);

public slots:
    void clear();

private:
    AbstractHistory *m_sourceHistory;
};

} // namespace GuiSystem

#endif // PROXYHISTORY_H
