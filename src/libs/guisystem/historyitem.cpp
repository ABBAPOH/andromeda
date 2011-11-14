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
    QString path;
    QVariantHash userData;
};

} // namespace GuiSystem

using namespace GuiSystem;

HistoryItem::HistoryItem(const QString &path) :
    data(new HistoryItemData)
{
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

void HistoryItem::setIcon(const QIcon &icon)
{
    data->icon = icon;
}

bool HistoryItem::isValid() const
{
    return !data->path.isEmpty();
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

QString HistoryItem::path() const
{
    return data->path;
}

void HistoryItem::setPath(const QString &newPath)
{
    if (path() == newPath)
        return;

    data->path = newPath;
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
