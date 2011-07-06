#include "navigationmodel.h"
#include "navigationmodel_p.h"

//#include <QDriveInfo>
//#include <QDriveController>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include <QSettings>

class QDriveInfo;

NavigationModelPrivate::NavigationModelPrivate(NavigationModel *qq) :
    q_ptr(qq)
{
}

void NavigationModelPrivate::insertItem(TreeItem *parentItem, const QString &name, const QString &path)
{
    Q_Q(NavigationModel);

    QModelIndex parent = q->createIndex(parentItem->row(), 0, parentItem);
    q->beginInsertRows(parent, parentItem->childCount(), parentItem->childCount());
    TreeItem *item = new TreeItem(parentItem, name, path);
    item->icon = iconProvider.icon(QFileInfo(path));
    mapToItem.insert(path, item);
    q->endInsertRows();
}

void NavigationModelPrivate::removeItem(const QString &path)
{
    Q_Q(NavigationModel);

    TreeItem *item = mapToItem.value(path);
    if (!item)
        return;

    TreeItem *parentItem = item->parent();

    QModelIndex parent = q->createIndex(parentItem->row(), 0, parentItem);
    q->beginRemoveRows(parent, item->row(), item->row());
    delete item;
    mapToItem.remove(path);
    q->endRemoveRows();
}

QString getDriveName(const QDriveInfo &info)
{
//    QString name = info.name();

//#if defined(Q_OS_WIN)
//    QString path = info.rootPath();
//    if (!name.isEmpty())
//        name = QString("%1 (%2)").arg(path).arg(name);
//    else
//        name = QString("%1").arg(path);
//#elif defined(Q_OS_LINUX)
//    QString path = info.rootPath();
//    if (name.isEmpty())
//        name = path;
//#endif

//    return name;
    return QString();
}

void NavigationModelPrivate::onDriveAdded(const QString &path)
{
//    QDriveInfo info(path);
//    QString name = getDriveName(info);

//    if (info.type() == QDriveInfo::RemoteDrive)
//        insertItem(networkItem, name, path);
//    else if (info.type() != QDriveInfo::InvalidDrive)
//        insertItem(drivesItem, name, path);
}

void NavigationModelPrivate::onDriveRemoved(const QString &path)
{
    removeItem(path);
}

NavigationModel::NavigationModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new NavigationModelPrivate(this))
{
    Q_D(NavigationModel);

    d->rootItem = new TreeItem();

    d->drivesItem = new TreeItem(d->rootItem, tr("Devices"));
    d->networkItem = new TreeItem(d->rootItem, tr("Network"));
    d->foldersItem = new TreeItem(d->rootItem, tr("Folders"));

//    d->driveController = new QDriveController(this);
//    connect(d->driveController, SIGNAL(driveMounted(QString)), d, SLOT(onDriveAdded(QString)));
//    connect(d->driveController, SIGNAL(driveUnmounted(QString)), d, SLOT(onDriveRemoved(QString)));

//    QList<QDriveInfo> drives = QDriveInfo::drives();
//    foreach (const QDriveInfo &info, drives) {
//        QString name = getDriveName(info);
//        QString path = info.rootPath();

//        TreeItem *item = 0;
//        if (info.type() == QDriveInfo::RemoteDrive)
//            item = new TreeItem(d->networkItem, name, path);
//        else if (info.type() != QDriveInfo::InvalidDrive)
//            item = new TreeItem(d->drivesItem, name, path);

//        if (item) {
//            item->icon = d->iconProvider.icon(QFileInfo(path));
//            d->mapToItem.insert(path, item);
//        }
//    }

    QSettings settings("NavigationModel");
    if (settings.contains("folders")) {
        QStringList folders = settings.value("folders").toStringList();
        foreach (const QString &folder, folders)
            addFolder(folder);
    } else {
        StandardLocations locations(DesktopLocation | DocumentsLocation |
                                    HomeLocation | ApplicationsLocation);
        setStandardLocations(locations);
    }
}

NavigationModel::~NavigationModel()
{
    Q_D(NavigationModel);

    QSettings settings("NavigationModel");
    QStringList folders;
    foreach (TreeItem *item, d->foldersItem->m_children) {
        folders.append(item->path);
    }
    settings.setValue("folders", folders);

    delete d->rootItem;

    delete d_ptr;
}

int NavigationModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant NavigationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return item->name;
    } else if (role == Qt::DecorationRole) {
        if (item->type == TreeItem::ChildItem)
            return item->icon;
        else
            return QVariant();
    }
    return QVariant();
}

QStringList NavigationModel::mimeTypes() const
{
    return QStringList() << "text/uri-list";
}

QMimeData * NavigationModel::mimeData(const QModelIndexList &indexes) const
{
    Q_D(const NavigationModel);

    QMimeData *data = new QMimeData();
    QList<QUrl> urls;
    foreach (const QModelIndex &index, indexes) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item->type == TreeItem::ChildItem && item->parent() == d->foldersItem) {
            urls.append(QUrl::fromLocalFile(item->path));
        }
    }
    data->setUrls(urls);
    return data;
}

#include <QDebug>
bool NavigationModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                   int row, int column, const QModelIndex &parent)
{
    qDebug() << "NavigationModel::dropMimeData" << row << column << parent;
    if (!data->hasUrls())
        return false;

    Q_D(NavigationModel);

    const QList<QUrl> & urls = data->urls();
    for (int i = 0; i < urls.size(); i++) {
        QString path = urls[i].toLocalFile();
        TreeItem *item = d->mapToItem.value(path);
        if (item) {
            beginRemoveRows(parent, item->row(), item->row());
            delete item;
            endRemoveRows();
        }
    }

    beginInsertRows(parent, row, row + urls.size());
    for (int i = 0; i < urls.size(); i++) {
        QString path = urls[i].toLocalFile();
        QFileInfo info(path);

        TreeItem *item = new TreeItem(d->foldersItem, row + i);
        item->type = TreeItem::ChildItem;
        item->path = path;
        item->name = info.fileName();
        item->icon = d->iconProvider.icon(info);

        d->mapToItem.insert(path, item);
    }
    endInsertRows();
    return true;
}

Qt::ItemFlags NavigationModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Q_D(const NavigationModel);

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item->type == TreeItem::ChildItem && item->parent() == d->foldersItem)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

    if (item == d->foldersItem)
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    if (item->type == TreeItem::ChildItem)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return Qt::ItemIsEnabled;
}

QModelIndex NavigationModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const NavigationModel);

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = d->rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex NavigationModel::parent(const QModelIndex &index) const
{
    Q_D(const NavigationModel);

    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int NavigationModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const NavigationModel);

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = d->rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

Qt::DropActions NavigationModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QModelIndex NavigationModel::index(const QString &path) const
{
    Q_D(const NavigationModel);

    TreeItem *item = d->mapToItem.value(path);
    if (!item)
        return QModelIndex();
    else
        return createIndex(item->row(), 0, item);
}

QString NavigationModel::path(const QModelIndex &index) const
{
    if (!index.isValid())
        return "";

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item->type == TreeItem::ChildItem)
        return item->path;
    else
        return "";
}

void NavigationModel::addFolder(const QString &path)
{
    Q_D(NavigationModel);

    QFileInfo info(path);
    if (!info.isDir() || !info.exists())
        return;

    QString canonicalPath = info.canonicalFilePath();
    QString name = info.fileName();

    if (d->mapToItem.contains(canonicalPath))
        return;

    d->insertItem(d->foldersItem, name, canonicalPath);
}

void NavigationModel::removeFolder(const QString &path)
{
    Q_D(NavigationModel);

    d->removeItem(path);
}

//NavigationModel::StandardLocations NavigationModel::standardLocations() const
//{
//    Q_D(const NavigationModel);

//    return d->locations;
//}

void NavigationModel::setStandardLocations(StandardLocations locations)
{
//    Q_D(NavigationModel);

//    d->locations = locations;

    QString path;

    path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    if (locations & DesktopLocation)
        addFolder(path);
    else
        removeFolder(path);

    path = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    if (locations & HomeLocation)
        addFolder(path);
    else
        removeFolder(path);

    path = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
    if (locations & ApplicationsLocation)
        addFolder(path);
    else
        removeFolder(path);

    path = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    if (locations & DocumentsLocation)
        addFolder(path);
    else
        removeFolder(path);

    path = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
    if (locations & MusicLocation)
        addFolder(path);
    else
        removeFolder(path);

    path = QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
    if (locations & MoviesLocation)
        addFolder(path);
    else
        removeFolder(path);

    path = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
    if (locations & PicturesLocation)
        addFolder(path);
    else
        removeFolder(path);
}
