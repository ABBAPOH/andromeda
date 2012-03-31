#ifndef COMMANDSMODEL_P_H
#define COMMANDSMODEL_P_H

#include "commandsmodel.h"

class QSettings;

namespace GuiSystem {

class Command;
struct CommandsModelItem
{
    enum Type { Root = 0, Folder, Leaf };

    explicit CommandsModelItem(Type type = Root, CommandsModelItem *parent = 0, int row = -1) :
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

    ~CommandsModelItem()
    {
        foreach (CommandsModelItem *item, m_children) {
            delete item;
        }
        if (m_parent) {
            m_parent->m_children.removeAll(this);
        }
    }

    inline CommandsModelItem *child(int row) const { return m_children.at(row); }
    inline int childCount() const { return m_children.count(); }
    inline QList<CommandsModelItem *> children() const { return m_children; }
    inline void insert(CommandsModelItem *item, int row) { item->m_parent = this; m_children.insert(row, item);}
    inline CommandsModelItem *parent() const { return m_parent; }
    inline void remove(CommandsModelItem *item) { m_children.removeAll(item); }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((CommandsModelItem *)this) : 0; }
    inline Type type() const { return m_type; }

private:
    CommandsModelItem *m_parent;
    QList<CommandsModelItem *> m_children;

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

    QModelIndex index(CommandsModelItem *item) const;
    CommandsModelItem *item(const QModelIndex &index) const;

    void build();

public:
    CommandsModelItem *rootItem;
    QSettings *settings;
    QMultiMap<QKeySequence, CommandsModelItem *> mapToItem;
    QMultiMap<QKeySequence, Command *> mapToCommand;
};

} // namespace GuiSystem

#endif // COMMANDSMODEL_P_H
