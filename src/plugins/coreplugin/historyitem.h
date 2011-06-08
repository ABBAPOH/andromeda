#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include "coreplugin_global.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

class HistoryItemData;
class COREPLUGIN_EXPORT HistoryItem
{
public:
    HistoryItem();
    HistoryItem(QString path,
                QString title = QString(),
                QIcon icon = QIcon(),
                QDateTime lastVisited = QDateTime::currentDateTime());
    HistoryItem(const HistoryItem &);
    HistoryItem &operator=(const HistoryItem &);
    ~HistoryItem();

    QIcon icon() const;
    bool isValid() const;
    QDateTime lastVisited() const;
    QString title() const;
    QString path() const;
    QVariant userData(const QString &key) const;
    QVariantHash &userData() const;
    void setUserData(const QString &key, const QVariant & data);

    bool operator==(const HistoryItem &other);

private:
    QExplicitlySharedDataPointer<HistoryItemData> data;
};

#endif // HISTORYITEM_H
