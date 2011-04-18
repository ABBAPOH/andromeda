#ifndef NAVIGATIONMODEL_P_H
#define NAVIGATIONMODEL_P_H

#include "navigationmodel.h"

#include <QIcon>
#include <QFileIconProvider>

struct TreeItem
{
    TreeItem *m_parent;
    QList<TreeItem *> m_children;

    enum Type { RootItem = 0, GroupItem, ChildItem };
    Type type;
    QString name;
    QString path;
    QIcon icon;

    explicit TreeItem(TreeItem *parent = 0)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.append(this);
        type = RootItem;
    }

    TreeItem(TreeItem *parent, const QString &name)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.append(this);

        type = TreeItem::GroupItem;
        this->name = name;
    }

    TreeItem(TreeItem *parent, const QString &name, const QString &path)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.append(this);

        type = TreeItem::ChildItem;
        this->name = name;
        this->path = path;
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

    int childCount()
    {
        return m_children.count();
    }

    TreeItem *parent()
    {
        return m_parent;
    }

    TreeItem *child(int row)
    {
        return m_children.at(row);
    }

    int row()
    {
        if (m_parent) {
            return m_parent->m_children.indexOf(this);
        }
        return 0;
    }
};

class QFileIconProvider;
class QDriveController;
class NavigationModelPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(NavigationModel)
    NavigationModel *q_ptr;

public:
    NavigationModelPrivate(NavigationModel *qq);

    TreeItem *rootItem;
    TreeItem *drivesItem;
    TreeItem *networkItem;
    TreeItem *foldersItem;

    QMap<QString, TreeItem*> mapToItem;

    QDriveController *driveController;

    QFileIconProvider iconProvider;

    NavigationModel::StandardLocations locations;

    void insertItem(TreeItem *parent, const QString &name, const QString &path);
    void removeItem(const QString &path);

public slots:
    void onDriveAdded(const QString &);
    void onDriveRemoved(const QString &);
};

#endif // NAVIGATIONMODEL_P_H
