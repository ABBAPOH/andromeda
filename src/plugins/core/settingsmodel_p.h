#ifndef SETTINGSMODEL_P_H
#define SETTINGSMODEL_P_H

#include "settingsmodel.h"

#include <QtGui/QIcon>

namespace Core {

struct SettingsModelItem
{
    enum Type { Root = 0, Folder, Leaf };

    explicit SettingsModelItem(Type type = Root, SettingsModelItem *parent = 0, int row = -1)
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

    ~SettingsModelItem()
    {
        foreach (SettingsModelItem *item, m_children) {
            delete item;
        }
        if (m_parent) {
            m_parent->m_children.removeAll(this);
        }
    }

    inline SettingsModelItem *child(int row) const { return m_children.at(row); }
    inline int childCount() const { return m_children.count(); }
    inline QList<SettingsModelItem *> children() const { return m_children; }
    inline void insert(SettingsModelItem *item, int row) { item->m_parent = this; m_children.insert(row, item);}
    inline SettingsModelItem *parent() const { return m_parent; }
    inline void remove(SettingsModelItem *item) { m_children.removeAll(item); }
    inline int row() const { return m_parent ? m_parent->m_children.indexOf((SettingsModelItem *)this) : 0; }
    inline Type type() const { return m_type; }
    inline void setType(Type type) { m_type = type; }

private:
    SettingsModelItem *m_parent;
    QList<SettingsModelItem *> m_children;

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

    QModelIndex index(SettingsModelItem *item) const;
    SettingsModelItem *item(const QModelIndex &index) const;

    SettingsModelItem *findItem(SettingsModelItem *parent, const QString &key);
    void moveItemUp(SettingsModelItem *parent, int oldRow, int newRow);
    void fillGroup(SettingsModelItem *parent);
    void refresh(SettingsModelItem *parent);
    void rebuild();
    void submit(SettingsModelItem *item);

public:
    SettingsModelItem *rootItem;

    QSettings *settings;
    SettingsModel::EditStrategy editStrategy;
    bool readOnly;
    QIcon dirIcon;
    QIcon keyIcon;
    QStringList keysToRemove;
};

} // namespace Core

#endif // SETTINGSMODEL_P_H
