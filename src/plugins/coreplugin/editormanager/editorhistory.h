#ifndef HISTORY_H
#define HISTORY_H

#include "../coreplugin_global.h"

#include <QtCore/QObject>

#include "historyitem.h"

namespace CorePlugin {

class EditorHistoryPrivate;
class COREPLUGIN_EXPORT EditorHistory: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorHistory)
    Q_DISABLE_COPY(EditorHistory)

    Q_PROPERTY(int currentItemIndex READ currentItemIndex NOTIFY currentItemIndexChanged)
    Q_PROPERTY(int maximumItemCount READ maximumItemCount WRITE setMaximumItemCount)
    Q_PROPERTY(bool canGoBack READ canGoBack)
    Q_PROPERTY(bool canGoForward READ canGoForward)
public:
    explicit EditorHistory(QObject *parent = 0);
    ~EditorHistory();

    void appendItem(const HistoryItem &item);

    bool canGoBack() const;
    bool canGoForward() const;

    HistoryItem backItem() const;
    QList<HistoryItem> backItems(int maxItems) const;

    void clear();

    int count() const;

    HistoryItem currentItem() const;
    int currentItemIndex() const;

    HistoryItem forwardItem() const;
    QList<HistoryItem> forwardItems(int maxItems) const;

    void goToItem(const HistoryItem & item);
    HistoryItem itemAt(int i) const;
    QList<HistoryItem> items() const;

    int maximumItemCount() const;
    void setMaximumItemCount(int count);

public slots:
    void back();
    void forward();

signals:
    void currentItemIndexChanged(int index);

protected:
    EditorHistoryPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // HISTORY_H
