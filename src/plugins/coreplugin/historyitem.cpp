#include "historyitem.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariant>

namespace CorePlugin {

class HistoryItemData : public QSharedData {
public:
    QIcon icon;
    bool valid;
    QDateTime lastVisited;
    QString title;
    QString path;
    QVariantHash userData;
};

} // namespace CorePlugin

using namespace CorePlugin;

HistoryItem::HistoryItem() :
    data(new HistoryItemData)
{
    data->valid = false;
}

HistoryItem::HistoryItem(QString path,
                         QString title,
                         QIcon icon,
                         QDateTime lastVisited) :
    data(new HistoryItemData)
{
    data->icon = icon;
    data->valid = true;
    data->lastVisited = lastVisited;
    data->title = title;
    data->path = path;
}

HistoryItem::HistoryItem(const HistoryItem &other) :
    data(other.data)
{
}

HistoryItem &HistoryItem::operator=(const HistoryItem &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

HistoryItem::~HistoryItem()
{
}

QIcon HistoryItem::icon() const
{
    return data->icon;
}

//void HistoryItem::setIcon(const QIcon &icon)
//{
//    data->icon = icon;
//}

bool HistoryItem::isValid() const
{
    return data->valid;
}

QDateTime HistoryItem::lastVisited() const
{
    return data->lastVisited;
}

QString HistoryItem::title() const
{
    return data->title;
}

QString HistoryItem::path() const
{
    return data->path;
}

QVariant HistoryItem::userData(const QString &key) const
{
    return data->userData.value(key);
}

QVariantHash &HistoryItem::userData() const
{
    return data->userData;
}

void HistoryItem::setUserData(const QString &key, const QVariant & data)
{
   this->data->userData[key] = data;
}

bool HistoryItem::operator==(const HistoryItem &other)
{
    return data == other.data;
}
