#include "historyitem.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariant>

class HistoryItemData : public QSharedData {
public:
    QIcon icon;
    bool valid;
    QDateTime lastVisited;
    QString title;
    QString path;
    QVariant userData;
//    QImage preview; // for future.
};

HistoryItem::HistoryItem() :
        data(new HistoryItemData)
{
    data->valid = false;
}

HistoryItem::HistoryItem(const HistoryItem &rhs) :
        data(rhs.data)
{
}

HistoryItem &HistoryItem::operator=(const HistoryItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

HistoryItem::~HistoryItem()
{
}

QIcon HistoryItem::icon() const
{
    return data->icon;
}

bool HistoryItem::isValid() const
{
    return data->valid;
}

QDateTime HistoryItem::lastVisited() const
{
    return data->lastVisited;
}

//QUrl originalUrl() const
//{
//}

void HistoryItem::setUserData(const QVariant & userData)
{
    data->userData = userData;
}

QString HistoryItem::title() const
{
    return data->title;
}

QString HistoryItem::path() const
{
    return data->path;
}

QVariant HistoryItem::userData() const
{
    return data->userData;
}

bool HistoryItem::operator==(const HistoryItem &other)
{
    return data->path == other.data->path;
}
