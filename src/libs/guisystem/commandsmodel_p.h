#ifndef COMMANDSMODEL_P_H
#define COMMANDSMODEL_P_H

#include "commandsmodel.h"

class QSettings;

namespace GuiSystem {

class Command;
struct TreeItem
{
    enum Type { Root = 0, Folder, Leaf };

    explicit TreeItem(Type type = Root, TreeItem *parent = 0, int row = -1) :
        cmd(0)
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
    inline TreeItem *parent() const { return m_parent; }
    inline void remove(TreeItem *item) { m_children.removeAll(item); }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((TreeItem *)this) : 0; }
    inline Type type() const { return m_type; }

private:
    TreeItem *m_parent;
    QList<TreeItem *> m_children;

    Type m_type;

public:
    QString name;
    Command *cmd;
};

class CommandsModelPrivate
{
    Q_DECLARE_PUBLIC(CommandsModel)
    Q_DISABLE_COPY(CommandsModelPrivate)

    CommandsModel *q_ptr;

public:
    explicit CommandsModelPrivate(CommandsModel *qq) : q_ptr(qq) {}

    QModelIndex index(TreeItem *item) const;
    TreeItem *item(const QModelIndex &index) const;

    void build();

public:
    TreeItem *rootItem;
    QSettings *settings;
    QMultiMap<QKeySequence, TreeItem *> mapToItem;
};

} // namespace GuiSystem

#endif // COMMANDSMODEL_P_H
