#ifndef BOOKMARKSMODEL_P_H
#define BOOKMARKSMODEL_P_H

#include "bookmarksmodel.h"

#include <QtGui/QUndoCommand>

namespace Bookmarks {

struct TreeItem
{
    enum Type { Root = 0, Folder, Item };

    explicit TreeItem(Type type = Root, TreeItem *parent = 0, int row = -1)
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

    ~TreeItem()
    {
        foreach (TreeItem *item, m_children) {
            delete item;
        }
        if (m_parent) {
            m_parent->m_children.removeAll(this);
        }
    }

    inline TreeItem *child(int row) const { return m_children.at(row); }
    inline int childCount() const { return m_children.count(); }
    inline QList<TreeItem *> children() const { return m_children; }
    inline void insert(TreeItem *item, int row) { item->m_parent = this; m_children.insert(row, item);}
    inline void remove(TreeItem *item) { m_children.removeAll(item); }
    inline TreeItem *parent() const { return m_parent; }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((TreeItem *)this) : 0; }
    inline Type type() const { return m_type; }

private:
    TreeItem *m_parent;
    QList<TreeItem *> m_children;

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

    QModelIndex index(TreeItem *item) const;
    TreeItem *item(const QModelIndex &index) const;

    void changeItem(TreeItem *item, const QVariant &value, int role);
    void insertItem(TreeItem *item, TreeItem *parent, int row);
    void removeItem(TreeItem *item);

    void readItems(QDataStream &s);
    void readRootItem(QDataStream &s, TreeItem *parent);
    void readItem(QDataStream &s, TreeItem *parent);
    void writeItems(QDataStream &s) const;
    void writeItem(QDataStream &s, const TreeItem *parent) const;

public:
    TreeItem *rootItem;
    TreeItem *menuItem;
    TreeItem *toolbarItem;
    TreeItem *bookmarksItem;

    QUndoStack *undoStack;
    bool endMacro;

private:
};

class InsertItemCommand : public QUndoCommand
{
public:
    InsertItemCommand(BookmarksModel *model, TreeItem *item, TreeItem *parentItem, int row);
    ~InsertItemCommand();

    void redo();
    void undo();

protected:
    BookmarksModel *model;
    TreeItem *item;
    TreeItem *parentItem;
    int row;
    bool done;
};

class RemoveItemCommand : public InsertItemCommand
{
public:
    RemoveItemCommand(BookmarksModel *model, TreeItem *item, TreeItem *parentItem, int row) :
        InsertItemCommand(model, item, parentItem, row)
    {}

    void redo() { InsertItemCommand::undo(); }
    void undo() { InsertItemCommand::redo(); }
};

class ChangeBookmarkCommand : public QUndoCommand
{
public:
    enum Role { TitleRole, UrlRole, DescriptionRole };
    ChangeBookmarkCommand(BookmarksModel *model, TreeItem *item, const QVariant &value, int role);

    void redo();
    void undo();

protected:
    BookmarksModel *model;
    TreeItem *item;
    QVariant newValue;
    QVariant oldValue;
    int role;
};

} // namespace Bookmarks

#endif // BOOKMARKSMODEL_P_H
