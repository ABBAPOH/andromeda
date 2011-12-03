#ifndef HISTORY_H
#define HISTORY_H

#include "guisystem_global.h"

#include <QtCore/QObject>

#include "historyitem.h"

namespace GuiSystem {

class HistoryPrivate;
class GUISYSTEM_EXPORT History: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(History)
    Q_DISABLE_COPY(History)

    Q_PROPERTY(int currentItemIndex READ currentItemIndex NOTIFY currentItemIndexChanged)
    Q_PROPERTY(int maximumItemCount READ maximumItemCount WRITE setMaximumItemCount)
    Q_PROPERTY(bool canGoBack READ canGoBack)
    Q_PROPERTY(bool canGoForward READ canGoForward)
public:
    explicit History(QObject *parent = 0);
    ~History();

    void appendItem(const HistoryItem &item);

    bool canGoBack() const;
    bool canGoForward() const;

    HistoryItem backItem() const;
    QList<HistoryItem> backItems(int maxItems) const;

    int count() const;

    HistoryItem currentItem() const;
    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    HistoryItem forwardItem() const;
    QList<HistoryItem> forwardItems(int maxItems) const;

    void goToItem(const HistoryItem & item);
    HistoryItem itemAt(int i) const;
    QList<HistoryItem> items() const;

    int maximumItemCount() const;
    void setMaximumItemCount(int count);

public slots:
    void back();
    void forward();

    void clear();

signals:
    void canGoBackChanged(bool);
    void canGoForwardChanged(bool);

    void currentItemIndexChanged(int index);

protected:
    HistoryPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // HISTORY_H
