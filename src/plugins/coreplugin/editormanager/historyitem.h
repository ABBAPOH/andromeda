#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtGui/QIcon>

class HistoryItemData;

class HistoryItem
{
public:
    HistoryItem();
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
