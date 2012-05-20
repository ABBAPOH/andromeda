#ifndef STACKEDHISTORY_H
#define STACKEDHISTORY_H

#include "ihistory.h"
#include "proxyeditor.h"

namespace GuiSystem {

class StackedContainer;

class StackedHistoryPrivate;
class StackedHistory : public IHistory
{
    Q_OBJECT

public:
    explicit StackedHistory(QObject *parent = 0);
    ~StackedHistory();

    void setContainer(StackedContainer *container);

    void open(const QUrl &url);

    int count() const;

    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    HistoryItem itemAt(int index) const;

    QByteArray store() const;
    void restore(const QByteArray &);

public slots:
    void clear();

private slots:
    void localHistoryIndexChanged(int index);
    void onUrlChanged(const QUrl &url);

private:
    StackedHistoryPrivate *d;
};

} // namespace GuiSystem

#endif // STACKEDHISTORY_H
