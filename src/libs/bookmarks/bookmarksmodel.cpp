#include "bookmarksmodel.h"
#include "bookmarksmodel_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QMimeData>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileIconProvider>
#include <QtGui/QUndoStack>

using namespace Bookmarks;

QModelIndex BookmarksModelPrivate::index(TreeItem *item) const
{
    return q_func()->createIndex(item->row(), 0, item);
}

TreeItem * BookmarksModelPrivate::item(const QModelIndex &index) const
{
    if (index.isValid())
        return static_cast<TreeItem*>(index.internalPointer());

    return rootItem;
}

void BookmarksModelPrivate::changeItem(TreeItem *item, const QVariant &value, int role)
{
    undoStack->push(new ChangeBookmarkCommand(q_func(), item, value, role));
}

void BookmarksModelPrivate::insertItem(TreeItem *item, TreeItem *parentItem, int row)
{
    undoStack->push(new InsertItemCommand(q_func(), item, parentItem, row));
}

void BookmarksModelPrivate::removeItem(TreeItem *item)
{
    undoStack->push(new RemoveItemCommand(q_func(), item, item->parent(), item->row()));
}

void BookmarksModelPrivate::readItems(QDataStream &s)
{
    readRootItem(s, menuItem);
    readRootItem(s, toolbarItem);
    readRootItem(s, bookmarksItem);
}

void BookmarksModelPrivate::readRootItem(QDataStream &s, TreeItem *parent)
{
    qint32 childCount;
    s >> childCount;
    s >> parent->name;
    if (childCount != -1) {
        for (int i = 0; i < childCount; i++) {
            readItem(s, parent);
        }
    }
}

void BookmarksModelPrivate::readItem(QDataStream &s, TreeItem *parent)
{
    qint32 childCount;
    s >> childCount;
    if (childCount != -1) {
        TreeItem *child = new TreeItem(TreeItem::Folder, parent);
        s >> child->name;
        for (int i = 0; i < childCount; i++) {
            readItem(s, child);
        }
    } else {
        TreeItem *child = new TreeItem(TreeItem::Item, parent);
        s >> child->bookmark;
    }
}

void BookmarksModelPrivate::writeItems(QDataStream &s) const
{
    writeItem(s, menuItem);
    writeItem(s, toolbarItem);
    writeItem(s, bookmarksItem);
}

void BookmarksModelPrivate::writeItem(QDataStream &s, const TreeItem *parent) const
{
    if (parent->type() == TreeItem::Item) {
        s << (qint32)-1;
        s << parent->bookmark;
    } else {
        s << parent->childCount();
        if (parent->type() == TreeItem::Folder)
            s << parent->name;
        foreach (TreeItem *child, parent->children()) {
            writeItem(s, child);
        }
    }
}

InsertItemCommand::InsertItemCommand(BookmarksModel *m, TreeItem *i, TreeItem *p, int r) :
    model(m),
    item(i),
    parentItem(p),
    row(r),
    done(false)
{
}

InsertItemCommand::~InsertItemCommand()
{
    if (!done && !item->parent()) // detached item
        delete item;
}

void InsertItemCommand::redo()
{
    model->beginInsertRows(model->d_func()->index(parentItem), row, row);
    parentItem->insert(item, row);
    model->endInsertRows();
    done = true;
}

void InsertItemCommand::undo()
{
    model->beginRemoveRows(model->d_func()->index(item->parent()), item->row(), item->row());
    item->parent()->remove(item);
    model->endRemoveRows();
    done = false;
}

ChangeBookmarkCommand::ChangeBookmarkCommand(BookmarksModel *m, TreeItem *i, const QVariant &v, int r) :
    model(m),
    item(i),
    newValue(v),
    role(r)
{
    switch (r) {
    case DescriptionRole : oldValue = i->bookmark.description(); break;
    case TitleRole : oldValue = (i->type() == TreeItem::Item) ? i->bookmark.title() : i->name; break;
    case UrlRole : oldValue = i->bookmark.url(); break;
    default: break;
    }
}

void ChangeBookmarkCommand::redo()
{
    switch (role) {
    case DescriptionRole :
        item->bookmark.setDescription(newValue.toString());
        break;
    case TitleRole :
        if (item->type() == TreeItem::Folder)
            item->name = newValue.toString();
        else
            item->bookmark.setTitle(newValue.toString());
        break;
    case UrlRole :
        item->bookmark.setUrl(newValue.toString());
        break;
    default:
        break;
    }

    QModelIndex parent = model->d_func()->index(item->parent());
    QModelIndex index = model->index(item->row(), role, parent);
    emit model->dataChanged(index, index);
}

void ChangeBookmarkCommand::undo()
{
    switch (role) {
    case DescriptionRole :
        item->bookmark.setDescription(oldValue.toString());
        break;
    case TitleRole :
        if (item->type() == TreeItem::Folder)
            item->name = oldValue.toString();
        else
            item->bookmark.setTitle(oldValue.toString());
        break;
    case UrlRole :
        item->bookmark.setUrl(oldValue.toString());
        break;
    default:
        break;
    }

    QModelIndex parent = model->d_func()->index(item->parent());
    QModelIndex index = model->index(item->row(), role, parent);
    emit model->dataChanged(index, index);
}

/*!
  \class BookmarksModel
*/
BookmarksModel::BookmarksModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new BookmarksModelPrivate(this))
{
    Q_D(BookmarksModel);

    d->rootItem = new TreeItem();

    d->menuItem = new TreeItem(TreeItem::Folder, d->rootItem, 0);
    d->toolbarItem = new TreeItem(TreeItem::Folder, d->rootItem, 1);
    d->bookmarksItem = new TreeItem(TreeItem::Folder, d->rootItem, 2);

    d->menuItem->name = tr("Bookmarks menu");
    d->toolbarItem->name = tr("Bookmarks toolbar");
    d->bookmarksItem->name = tr("Bookmarks");

    d->undoStack = new QUndoStack(this);
    d->endMacro = false;
}

BookmarksModel::~BookmarksModel()
{
    delete d_func()->rootItem;
    delete d_ptr;
}

int BookmarksModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant BookmarksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (item->type() == TreeItem::Item) {
            if (index.column() == 0)
                return item->bookmark.title();
            else if (index.column() == 1)
                return item->bookmark.url();
            else if (index.column() == 2)
                return item->bookmark.description();
        } else {
            if (index.column() == 0)
                return item->name;
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            if (item->type() == TreeItem::Item)
                return item->bookmark.icon();
            else
                return QFileIconProvider().icon(QFileIconProvider::Folder);
        }
    } else if (role == DescriptionRole) {
        if (item->type() == TreeItem::Item)
            return item->bookmark.description();
    } else if (role == PreviewRole) {
        if (item->type() == TreeItem::Item)
            return item->bookmark.preview();
    } else if (role == UrlRole) {
        if (item->type() == TreeItem::Item)
            return item->bookmark.url();
    }
    return QVariant();
}

Qt::ItemFlags BookmarksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    TreeItem *item = d_func()->item(index);
    TreeItem::Type type = item->type();
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (hasChildren(index))
        flags |= Qt::ItemIsDropEnabled;

    flags |= Qt::ItemIsDragEnabled;

    if ((index.column() == 0) ||
            ((index.column() == 1 || index.column() == 2) && type == TreeItem::Item))
        flags |= Qt::ItemIsEditable;

    return flags;
}

bool BookmarksModel::hasChildren(const QModelIndex &parent) const
{
    return d_func()->item(parent)->childCount() > 0;
}

QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("Title");
        case 1: return tr("Address");
        case 2: return tr("Description");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex BookmarksModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = d_func()->item(parent);
    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex BookmarksModel::parent(const QModelIndex &index) const
{
    Q_D(const BookmarksModel);

    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

bool BookmarksModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_D(BookmarksModel);

    if (row < 0 || count <= 0 || row + count > rowCount(parent))
        return false;

    TreeItem *parentItem = d->item(parent);
    for (int i = row + count - 1; i >= row; --i) {
        TreeItem *childItem = parentItem->child(i);
        d->removeItem(childItem);
    }

    if (d->endMacro) {
        undoStack()->endMacro();
        d->endMacro = false;
    }

    return true;
}

int BookmarksModel::rowCount(const QModelIndex &parent) const
{
    return d_func()->item(parent)->childCount();
}

bool BookmarksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(BookmarksModel);

    if (!index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0)
        return false;

    TreeItem *item = d->item(index);

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            d->changeItem(item, value, ChangeBookmarkCommand::TitleRole);
            break;
        case 1:
            d->changeItem(item, value, ChangeBookmarkCommand::UrlRole);
            break;
        case 2:
            d->changeItem(item, value, ChangeBookmarkCommand::DescriptionRole);
            break;
        default:
            return false;
        }
        break;
    case BookmarksModel::DescriptionRole:
        item->bookmark.setDescription(value.toString());
        emit dataChanged(index, index);
        break;
    case BookmarksModel::PreviewRole:
        item->bookmark.setPreview(value.value<QImage>());
        emit dataChanged(index, index);
        break;
    case BookmarksModel::UrlRole:
        item->bookmark.setUrl(value.toUrl());
        emit dataChanged(index, index);
        break;
    default:
        break;
        return false;
    }

    return true;
}

QStringList BookmarksModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/bookmarks.bin");
    types << QLatin1String("text/uri-list");
    return types;
}

QMimeData * BookmarksModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();

    QList<QUrl> urls;
    QByteArray data;

    QDataStream stream(&data, QIODevice::WriteOnly);
    foreach (const QModelIndex &index, indexes) {
        if (index.column() != 0 || !index.isValid())
            continue;

        urls.append(index.data(BookmarksModel::UrlRole).toUrl());

        TreeItem *item = d_func()->item(index);
        d_func()->writeItem(stream, item);
    }

    mimeData->setUrls(urls);
    mimeData->setData(QLatin1String("application/bookmarks.bin"), data);

    return mimeData;
}

bool BookmarksModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                  int row, int column, const QModelIndex &parent)
{
    Q_D(BookmarksModel);

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    if (!data->hasFormat(QLatin1String("application/bookmarks.bin"))) {
        if (!data->hasUrls())
            return false;

        Bookmark bookmark;
        bookmark.setUrl(data->urls().at(0));

        if (data->hasText())
            bookmark.setTitle(data->text());
        else
            bookmark.setTitle(bookmark.url().toString());

        addBookmark(bookmark, parent, row);
        return true;
    }

    QByteArray ba = data->data(QLatin1String("application/bookmarks.bin"));
    QDataStream stream(&ba, QIODevice::ReadOnly);
    if (stream.atEnd())
        return false;

    d->undoStack->beginMacro(QLatin1String("Move Bookmarks"));
    d->endMacro = true;

    while (!stream.atEnd()) {
        TreeItem *rootNode = new TreeItem;
        d->readItem(stream, rootNode);
        QList<TreeItem*> children = rootNode->children();
        for (int i = 0; i < children.count(); ++i) {
            TreeItem *item = children.at(i);
            rootNode->remove(item);
            row = qMax(0, row);

            TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
            d->insertItem(item, parentItem, row);
        }
        delete rootNode;
    }

    return true;
}

Qt::DropActions BookmarksModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

QModelIndex BookmarksModel::bookmarks() const
{
    Q_D(const BookmarksModel);

    return d->index(d->bookmarksItem);
}

QModelIndex BookmarksModel::menu() const
{
    Q_D(const BookmarksModel);

    return d->index(d->menuItem);
}

QModelIndex BookmarksModel::toolBar() const
{
    Q_D(const BookmarksModel);

    return d->index(d->toolbarItem);
}

bool BookmarksModel::isFolder(const QModelIndex &index) const
{
    if (!index.isValid())
        return true;

    return d_func()->item(index)->type() == TreeItem::Folder;
}

QModelIndex BookmarksModel::index(const QString &path)
{
    QStringList paths = path.split(QLatin1Char('/'), QString::SkipEmptyParts);
    QModelIndex result;
    foreach (const QString &path, paths) {
        for (int i = 0; i < rowCount(result); i++) {
            QModelIndex idx = index(i, 0, result);
            if (idx.data() == path) {
                result = idx;
                break;
            }
        }
    }
    return result;
}

Bookmark BookmarksModel::bookmark(const QModelIndex &index) const
{
    return d_func()->item(index)->bookmark;
}

QModelIndex BookmarksModel::addBookmark(const Bookmark &bookmark, const QModelIndex &parent, int row)
{
    Q_D(BookmarksModel);

    if (!parent.isValid())
        return QModelIndex();

    TreeItem *parentItem = d->item(parent);

    if (parentItem->type() != TreeItem::Folder)
        return QModelIndex();

    row = (row == -1) ? parentItem->childCount() : row;

    TreeItem *item = new TreeItem(TreeItem::Item);
    item->bookmark = bookmark;
    d->insertItem(item, parentItem, row);

    return index(row, 0, parent);
}

QModelIndex BookmarksModel::addFolder(const QString &folder, const QModelIndex &parent, int row)
{
    Q_D(BookmarksModel);

    TreeItem *parentItem = d->item(parent);

    row = (row == -1) ? parentItem->childCount() : row;

    TreeItem *item = new TreeItem(TreeItem::Folder);
    item->name = folder;
    d->insertItem(item, parentItem, row);

    return index(row, 0, parent);
}

void BookmarksModel::remove(const QModelIndex &index)
{
    Q_D(BookmarksModel);

    if (!index.isValid())
        return;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    d->removeItem(item);
}

QUndoStack *BookmarksModel::undoStack() const
{
    return d_func()->undoStack;
}

inline QString getBookmarksPath()
{
    QString dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    return dataLocation + QLatin1Char('/') + QLatin1String("bookmarks");
}

bool BookmarksModel::loadBookmarks()
{
    return loadBookmarks(getBookmarksPath());
}

bool BookmarksModel::loadBookmarks(const QString &file)
{
    QFile f(file);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return false;
    return loadBookmarks(&f);
}

bool BookmarksModel::loadBookmarks(QIODevice *device)
{
    QDataStream s(device);
    d_func()->readItems(s);
    reset();
    return true;
}

bool BookmarksModel::saveBookmarks()
{
    return saveBookmarks(getBookmarksPath());
}

bool BookmarksModel::saveBookmarks(const QString &file) const
{
    QFileInfo info(file);
    QDir dir = info.dir();
    if (!dir.exists())
        if (!dir.mkpath(dir.absolutePath()))
            return false;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    return saveBookmarks(&f);
}

bool BookmarksModel::saveBookmarks(QIODevice *device) const
{
    QDataStream s(device);
    d_func()->writeItems(s);

    return true;
}
