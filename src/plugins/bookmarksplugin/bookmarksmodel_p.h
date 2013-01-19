#ifndef BOOKMARKSMODEL_P_H
#define BOOKMARKSMODEL_P_H

#include "bookmarksmodel.h"

#include <QtGui/QUndoCommand>

namespace Bookmarks {

struct BookmarksModelItem
{
    enum Type { Root = 0, Folder, Item };

    explicit BookmarksModelItem(Type type = Root, BookmarksModelItem *parent = 0, int row = -1)
    {
        m_type = type;
        m_parent = parent;
        if (parent) {
            if (row == -1)
                parent->m_children.append(this);
            else
                parent->m_children.insert(row, this);
        }
    }

    ~BookmarksModelItem()
    {
        foreach (BookmarksModelItem *item, m_children) {
            delete item;
        }
        if (m_parent) {
            m_parent->m_children.removeAll(this);
        }
    }

    inline BookmarksModelItem *child(int row) const { return m_children.at(row); }
    inline int childCount() const { return m_children.count(); }
    inline QList<BookmarksModelItem *> children() const { return m_children; }
    inline void insert(BookmarksModelItem *item, int row) { item->m_parent = this; m_children.insert(row, item);}
    inline void remove(BookmarksModelItem *item) { m_children.removeAll(item); }
    inline BookmarksModelItem *parent() const { return m_parent; }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((BookmarksModelItem *)this) : 0; }
    inline Type type() const { return m_type; }

private:
    BookmarksModelItem *m_parent;
    QList<BookmarksModelItem *> m_children;

    Type m_type;

public:
    QString name;
    bool readOnly;
    Bookmark bookmark;
};

class BookmarksModelPrivate
{
    Q_DECLARE_PUBLIC(BookmarksModel)
    Q_DISABLE_COPY(BookmarksModelPrivate)

    BookmarksModel *q_ptr;

public:
    BookmarksModelPrivate(BookmarksModel *qq) : q_ptr(qq) {}

    QModelIndex index(BookmarksModelItem *item) const;
    BookmarksModelItem *item(const QModelIndex &index) const;

    void changeItem(BookmarksModelItem *item, const QVariant &value, int role);
    void insertItem(BookmarksModelItem *item, BookmarksModelItem *parent, int row);
    void removeItem(BookmarksModelItem *item);

    void readItems(QDataStream &s);
    void readRootItem(QDataStream &s, BookmarksModelItem *parent);
    void readItem(QDataStream &s, BookmarksModelItem *parent);
    void writeItems(QDataStream &s) const;
    void writeItem(QDataStream &s, const BookmarksModelItem *parent) const;

public:
    BookmarksModelItem *rootItem;
    BookmarksModelItem *menuItem;
    BookmarksModelItem *toolbarItem;
    BookmarksModelItem *bookmarksItem;

    QUndoStack *undoStack;
    bool endMacro;

private:
};

class InsertItemCommand : public QUndoCommand
{
public:
    InsertItemCommand(BookmarksModel *model, BookmarksModelItem *item, BookmarksModelItem *parentItem, int row);
    ~InsertItemCommand();

    void redo();
    void undo();

protected:
    BookmarksModel *model;
    BookmarksModelItem *item;
    BookmarksModelItem *parentItem;
    int row;
    bool done;
};

class RemoveItemCommand : public InsertItemCommand
{
public:
    RemoveItemCommand(BookmarksModel *model, BookmarksModelItem *item, BookmarksModelItem *parentItem, int row) :
        InsertItemCommand(model, item, parentItem, row)
    {}

    void redo() { InsertItemCommand::undo(); }
    void undo() { InsertItemCommand::redo(); }
};

class ChangeBookmarkCommand : public QUndoCommand
{
public:
    enum Role { TitleRole, UrlRole, DescriptionRole };
    ChangeBookmarkCommand(BookmarksModel *model, BookmarksModelItem *item, const QVariant &value, int role);

    void redo();
    void undo();

protected:
    BookmarksModel *model;
    BookmarksModelItem *item;
    QVariant newValue;
    QVariant oldValue;
    int role;
};

} // namespace Bookmarks

#endif // BOOKMARKSMODEL_P_H
