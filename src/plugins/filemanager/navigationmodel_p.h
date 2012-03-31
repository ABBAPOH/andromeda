#ifndef NAVIGATIONMODEL_P_H
#define NAVIGATIONMODEL_P_H

#include "navigationmodel.h"

#include <QIcon>
#include <QFileIconProvider>

class QDriveController;

namespace FileManager {

struct NavigationModelItem
{
    NavigationModelItem *m_parent;
    QList<NavigationModelItem *> m_children;

    enum Type { RootItem = 0, GroupItem, ChildItem };
    Type type;
    QString name;
    QString path;
    QIcon icon;
    QDriveInfo driveInfo;

    explicit NavigationModelItem(NavigationModelItem *parent = 0)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.append(this);
        type = RootItem;
    }

    NavigationModelItem(NavigationModelItem *parent, int row)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.insert(row, this);
    }

    NavigationModelItem(NavigationModelItem *parent, const QString &name)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.append(this);

        type = NavigationModelItem::GroupItem;
        this->name = name;
    }

    NavigationModelItem(NavigationModelItem *parent, const QString &name, const QString &path)
    {
        m_parent = parent;
        if (parent)
            parent->m_children.append(this);

        type = NavigationModelItem::ChildItem;
        this->name = name;
        this->path = path;
    }

    ~NavigationModelItem()
    {
        foreach (NavigationModelItem *item, m_children) {
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

    NavigationModelItem *parent()
    {
        return m_parent;
    }

    NavigationModelItem *child(int row)
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

class NavigationModelPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(NavigationModel)
    NavigationModel *q_ptr;

public:
    NavigationModelPrivate(NavigationModel *qq);

    NavigationModelItem *rootItem;
    NavigationModelItem *drivesItem;
    NavigationModelItem *networkItem;
    NavigationModelItem *foldersItem;

    QMap<QString, NavigationModelItem*> mapToItem;

    QDriveController *driveController;

    QFileIconProvider iconProvider;

    NavigationModel::StandardLocations locations;

    void insertItem(NavigationModelItem *parent, const QString &name, const QString &path);
    void removeItem(const QString &path);

public slots:
    void onDriveAdded(const QString &);
    void onDriveRemoved(const QString &);
};

} // namespace FileManager

#endif // NAVIGATIONMODEL_P_H
