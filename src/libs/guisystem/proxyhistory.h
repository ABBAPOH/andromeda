#ifndef PROXYHISTORY_H
#define PROXYHISTORY_H

#include "ihistory.h"

namespace GuiSystem {

class GUISYSTEM_EXPORT ProxyHistory : public IHistory
{
    Q_OBJECT
    Q_DISABLE_COPY(ProxyHistory)

public:
    explicit ProxyHistory(QObject *parent = 0);

    int count() const;

    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    HistoryItem itemAt(int index) const;

    IHistory *sourceHistory() const;
    void setSourceHistory(IHistory *history);

    QByteArray store() const { return QByteArray(); }
    void restore(const QByteArray &) {}

public slots:
    void clear();

private:
    IHistory *m_sourceHistory;
};

} // namespace GuiSystem

#endif // PROXYHISTORY_H
