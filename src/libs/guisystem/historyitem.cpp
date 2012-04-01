#include "historyitem.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariant>

namespace GuiSystem {

class HistoryItemData : public QSharedData {
public:
    QIcon icon;
    bool valid;
    QDateTime lastVisited;
    QString title;
    QUrl url;
    QVariantHash userData;
};

} // namespace GuiSystem

using namespace GuiSystem;

HistoryItem::HistoryItem(const QString &path) :
    data(new HistoryItemData)
{
    data->url = path;
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

void HistoryItem::setIcon(const QIcon &icon)
{
    data->icon = icon;
}

bool HistoryItem::isValid() const
{
    return !data->url.isEmpty();
}

QDateTime HistoryItem::lastVisited() const
{
    return data->lastVisited;
}

void HistoryItem::setLastVisited(const QDateTime &visited)
{
    if (lastVisited() == visited)
        return;

    data->lastVisited = visited;
}

QString HistoryItem::title() const
{
    return data->title;
}

void HistoryItem::setTitle(const QString &newTitle)
{
    if (title() == newTitle)
        return;

    data->title = newTitle;
}

QUrl HistoryItem::url() const
{
    return data->url;
}

void HistoryItem::setUrl(const QUrl &newPath)
{
    if (url() == newPath)
        return;

    data->url = newPath;
}

QVariant HistoryItem::userData(const QString &key) const
{
    return data->userData.value(key);
}

QVariantHash HistoryItem::userData() const
{
    return data->userData;
}

void HistoryItem::setUserData(const QString &key, const QVariant & data)
{
   this->data->userData[key] = data;
}

bool HistoryItem::operator==(const HistoryItem &other) const
{
    return data == other.data;
}
