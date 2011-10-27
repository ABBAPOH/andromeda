#ifndef BOOKMARKSMODEL_H
#define BOOKMARKSMODEL_H

#include "bookmarks_global.h"
#include "bookmark.h"

#include <QtCore/QAbstractItemModel>

class QUndoStack;

namespace Bookmarks {

class TreeItem;

class BookmarksModelPrivate;
class BOOKMARKS_EXPORT BookmarksModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BookmarksModel)
    Q_DISABLE_COPY(BookmarksModel)

public:
    enum Roles {
        DescriptionRole = Qt::UserRole + 1,
        PreviewRole = Qt::UserRole + 2,
        UrlRole = Qt::UserRole + 3
    };

    explicit BookmarksModel(QObject *parent = 0);
    ~BookmarksModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;

    QModelIndex bookmarks() const;
    QModelIndex menu() const;
    QModelIndex toolBar() const;

    bool isFolder(const QModelIndex &index) const;
    QModelIndex index(const QString &path);

    Bookmark bookmark(const QModelIndex &index) const;
    QModelIndex addBookmark(const Bookmark &bookmark, const QModelIndex &parent, int row = -1);
    QModelIndex addFolder(const QString &folder, const QModelIndex &parent, int row = -1);
    void remove(const QModelIndex &index);

    QUndoStack *undoStack() const;

    bool loadBookmarks();
    bool loadBookmarks(const QString &file);
    bool loadBookmarks(QIODevice *device);
    bool saveBookmarks();
    bool saveBookmarks(const QString &file) const;
    bool saveBookmarks(QIODevice *device) const;

protected:
    BookmarksModelPrivate *d_ptr;

    friend class InsertItemCommand;
    friend class ChangeBookmarkCommand;
};

} // namespace Bookmarks

#endif // BOOKMARKSMODEL_H
