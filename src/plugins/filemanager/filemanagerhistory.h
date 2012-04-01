#ifndef FILEMANAGERHISTORY_H
#define FILEMANAGERHISTORY_H

#include <QtCore/QObject>

#include "filemanagerhistoryitem.h"

namespace FileManager {

class FileManagerHistoryPrivate;
class FileManagerHistory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileManagerHistory)
    Q_DISABLE_COPY(FileManagerHistory)

    Q_PROPERTY(int currentItemIndex READ currentItemIndex NOTIFY currentItemIndexChanged)
    Q_PROPERTY(int maximumItemCount READ maximumItemCount WRITE setMaximumItemCount)
    Q_PROPERTY(bool canGoBack READ canGoBack)
    Q_PROPERTY(bool canGoForward READ canGoForward)

public:
    explicit FileManagerHistory(QObject *parent = 0);
    ~FileManagerHistory();

    bool canGoBack() const;
    bool canGoForward() const;

    FileManagerHistoryItem backItem() const;
    QList<FileManagerHistoryItem> backItems(int maxItems) const;

    int count() const;

    FileManagerHistoryItem currentItem() const;
    int currentItemIndex() const;
    void setCurrentItemIndex(int index);

    FileManagerHistoryItem forwardItem() const;
    QList<FileManagerHistoryItem> forwardItems(int maxItems) const;

    void goToItem(const FileManagerHistoryItem & item);
    FileManagerHistoryItem itemAt(int i) const;
    QList<FileManagerHistoryItem> items() const;

    int maximumItemCount() const;
    void setMaximumItemCount(int count);

public slots:
    void back();
    void forward();

    void clear();

signals:
    void canGoBackChanged(bool);
    void canGoForwardChanged(bool);

    void currentItemIndexChanged(int index);

protected:
    FileManagerHistoryPrivate *d_ptr;

    friend class FileManagerWidget;

    friend QDataStream &operator<<(QDataStream & stream, const FileManagerHistory & history);
    friend QDataStream &operator>>(QDataStream & stream, FileManagerHistory & history);
};

QDataStream &operator<<(QDataStream & stream, const FileManagerHistory & history);
QDataStream &operator>>(QDataStream & stream, FileManagerHistory & history);

} // namespace FileManager

#endif // FILEMANAGERHISTORY_H
