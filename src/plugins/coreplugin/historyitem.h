#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include "../coreplugin_global.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

class HistoryItemData;

class COREPLUGIN_EXPORT HistoryItem
{
public:
    HistoryItem();
    HistoryItem (QIcon icon,
                 QDateTime lastVisited,
                 QString title,
                 QString path,
                 QVariant userData = QVariant());
    HistoryItem(const HistoryItem &);
    HistoryItem &operator=(const HistoryItem &);
    ~HistoryItem();

    QIcon icon() const;
    bool isValid() const;
    QDateTime lastVisited() const;
//    QUrl originalUrl() const;
    void setUserData(const QVariant & userData);
    QString title() const;
    QString path() const;
    QVariant userData() const;

    bool operator==(const HistoryItem &other);

private:
    QExplicitlySharedDataPointer<HistoryItemData> data;
};

#endif // HISTORYITEM_H
