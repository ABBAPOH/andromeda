#ifndef SETTINGSMODEL_P_H
#define SETTINGSMODEL_P_H

#include "settingsmodel.h"

#include <QtGui/QIcon>

struct TreeItem
{
    enum Type { Root = 0, Folder, Leaf };

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
    inline TreeItem *parent() const { return m_parent; }
    inline void remove(TreeItem *item) { m_children.removeAll(item); }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((TreeItem *)this) : 0; }
    inline Type type() const { return m_type; }
    inline void setType(Type type) { m_type = type; }

private:
    TreeItem *m_parent;
    QList<TreeItem *> m_children;

    Type m_type;

public:
    QString key;
    QVariant::Type valueType;
    QVariant value;
    QString longKey;
};

class SettingsModelPrivate
{
    Q_DECLARE_PUBLIC(SettingsModel)
    Q_DISABLE_COPY(SettingsModelPrivate)

    SettingsModel *q_ptr;

public:
    explicit SettingsModelPrivate(SettingsModel *qq) : q_ptr(qq) {}

    QModelIndex index(TreeItem *item) const;
    TreeItem *item(const QModelIndex &index) const;

    TreeItem *findItem(TreeItem *parent, const QString &key);
    void moveItemUp(TreeItem *parent, int oldRow, int newRow);
    void fillGroup(TreeItem *parent);
    void refresh(TreeItem *parent);
    void rebuild();
    void submit(TreeItem *item);

public:
    TreeItem *rootItem;

    QSettings *settings;
    SettingsModel::EditStrategy editStrategy;
    bool readOnly;
    QIcon dirIcon;
    QIcon keyIcon;
    QStringList keysToRemove;
};

#endif // SETTINGSMODEL_P_H
