#ifndef GLOBALHISTORY_H
#define GLOBALHISTORY_H

#include <QtCore/QObject>

#include "historyitem.h"

namespace CorePlugin {

class IHistoryView;

struct GlobalHistoryItem
{
    enum Type {Item = 1, View = 2 } type;

    GlobalHistoryItem() {}
    GlobalHistoryItem(const HistoryItem &item)
    {
        type = Item;
        this->item = new HistoryItem(item);
    }

    ~GlobalHistoryItem()
    {
        if (type == Item)
            delete item;
    }

    union {
        int index;
        HistoryItem *item;
        IHistoryView *view;
    };
};

class GlobalHistoryPrivate;
class GlobalHistory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(GlobalHistory)
public:
    explicit GlobalHistory(QObject *parent = 0);
    ~GlobalHistory();

    void appendItem(const HistoryItem &item);
    void appendView(IHistoryView *view);

    HistoryItem *currentItem() const;
    int currentIndex() const;

signals:
    void currentIndexChanged();

public slots:
    void back();
    void forward();

protected:
    GlobalHistoryPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // GLOBALHISTORY_H
