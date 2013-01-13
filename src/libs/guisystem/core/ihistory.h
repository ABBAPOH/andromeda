#ifndef IHISTORY_H
#define IHISTORY_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

#include <GuiSystem/HistoryItem>

namespace GuiSystem {

class GUISYSTEM_EXPORT IHistory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(IHistory)

    Q_PROPERTY(int currentItemIndex READ currentItemIndex WRITE setCurrentItemIndex NOTIFY currentItemIndexChanged)

public:
    explicit IHistory(QObject *parent = 0);

    virtual int count() const = 0;

    virtual int currentItemIndex() const = 0;
    virtual void setCurrentItemIndex(int index) = 0;

    virtual HistoryItem itemAt(int index) const = 0;

    virtual QByteArray store() const = 0;
    virtual void restore(const QByteArray &) = 0;

public slots:
    virtual void clear() = 0;

signals:
    void currentItemIndexChanged(int index);
};

} // namespace GuiSystem

#endif // IHISTORY_H
