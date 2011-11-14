#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include "guisystem_global.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QHash>

namespace GuiSystem {

class HistoryItemData;
class GUISYSTEM_EXPORT HistoryItem
{
public:
    HistoryItem(const QString &path = QString());
    HistoryItem(const HistoryItem &);
    HistoryItem &operator=(const HistoryItem &);
    ~HistoryItem();

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    bool isValid() const;

    QDateTime lastVisited() const;
    void setLastVisited(const QDateTime &visited);

    QString title() const;
    void setTitle(const QString &title);

    QString path() const;
    void setPath(const QString &path);

    QVariant userData(const QString &key) const;
    QVariantHash userData() const;
    void setUserData(const QString &key, const QVariant &data);

    bool operator==(const HistoryItem &other) const;

private:
    QSharedDataPointer<HistoryItemData> data;
};

} // namespace GuiSystem

#endif // HISTORYITEM_H
