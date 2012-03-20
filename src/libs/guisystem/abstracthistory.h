#ifndef ABSTRACTHISTORY_H
#define ABSTRACTHISTORY_H

#include "guisystem_global.h"

#include <QtCore/QObject>

#include "historyitem.h"

namespace GuiSystem {

class GUISYSTEM_EXPORT AbstractHistory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractHistory)

    Q_PROPERTY(int currentItemIndex READ currentItemIndex WRITE setCurrentItemIndex NOTIFY currentItemIndexChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack)
    Q_PROPERTY(bool canGoForward READ canGoForward)

public:
    explicit AbstractHistory(QObject *parent = 0);

    bool canGoBack() const;
    bool canGoForward() const;

    virtual int count() const = 0;

    virtual int currentItemIndex() const = 0;
    virtual void setCurrentItemIndex(int index) = 0;

    virtual HistoryItem itemAt(int index) const = 0;

public slots:
    void back();
    void forward();

    virtual void clear() = 0;

signals:
    void canGoBackChanged(bool);
    void canGoForwardChanged(bool);

    void currentItemIndexChanged(int index);

protected:
    void emitCurrentItemIndexChanged(int index, int oldIndex);
};

} // namespace GuiSystem

#endif // ABSTRACTHISTORY_H
