#include "settingsmodel.h"
#include "settingsmodel_p.h"

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtGui/QApplication>
#include <QtGui/QStyle>

using namespace Core;

QModelIndex SettingsModelPrivate::index(SettingsModelItem *item) const
{
    return q_func()->createIndex(item->row(), 0, item);
}

SettingsModelItem * SettingsModelPrivate::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return rootItem;

    return static_cast<SettingsModelItem*>(index.internalPointer());
}

SettingsModelItem *SettingsModelPrivate::findItem(SettingsModelItem *parent, const QString &key)
{
    for (int i = 0; i < parent->childCount(); i++) {
        SettingsModelItem *item = parent->child(i);
        if (item->key == key)
            return item;
    }
    return 0;
}

void SettingsModelPrivate::moveItemUp(SettingsModelItem *parent, int oldRow, int newRow)
{
    Q_Q(SettingsModel);
    if (oldRow == newRow)
        return;

    q->beginRemoveRows(index(parent), newRow, oldRow - 1);
    for (int i = 0; i < oldRow - newRow; i++) {
        delete parent->child(newRow);
    }
    q->endRemoveRows();
}

static QString getLongKey(const QString &group, const QString &key)
{
    return group.isEmpty() ? key : QString(QLatin1String("%1/%2")).arg(group).arg(key);
}

void SettingsModelPrivate::fillGroup(SettingsModelItem *parent)
{
    foreach (const QString &key, settings->childGroups()) {
        SettingsModelItem *item = new SettingsModelItem(SettingsModelItem::Folder, parent);
        item->key = key;
        item->longKey = getLongKey(settings->group(), key);

        settings->beginGroup(key);
        fillGroup(item);
        settings->endGroup();
    }

    foreach (const QString &key, settings->childKeys()) {
        SettingsModelItem *item = new SettingsModelItem(SettingsModelItem::Leaf, parent);
        item->key = key;
        QVariant value = settings->value(key);
        item->valueType = value.type();
        item->value = value;
        item->longKey = getLongKey(settings->group(), key);
    }
}

void SettingsModelPrivate::refresh(SettingsModelItem *parent)
{
    Q_Q(SettingsModel);

    int row = 0;
    foreach (const QString &key, settings->childGroups()) {
        SettingsModelItem *item = findItem(parent, key);
        if (item) {
            item->setType(SettingsModelItem::Folder);
            item->valueType = QVariant::Invalid;
            item->value = QVariant();
            moveItemUp(parent, item->row(), row);
        } else {
            q->beginInsertRows(index(parent), row, row);
            item = new SettingsModelItem(SettingsModelItem::Folder, parent, row);
            q->endInsertRows();
        }

        item->key = key;
        item->longKey = getLongKey(settings->group(), key);

        row++;
        settings->beginGroup(key);
        refresh(item);
        settings->endGroup();
    }

    foreach (const QString &key, settings->childKeys()) {
        SettingsModelItem *item = findItem(parent, key);
        if (item) {
            item->setType(SettingsModelItem::Leaf);
            moveItemUp(parent, item->row(), row);
        } else {
            q->beginInsertRows(index(parent), row, row);
            item = new SettingsModelItem(SettingsModelItem::Leaf, parent, row);
            q->endInsertRows();
        }

        item->key = key;
        QVariant value = settings->value(key);
        item->valueType = value.type();
        item->value = value;
        item->longKey = getLongKey(settings->group(), key);

        row++;
    }

    if (row < parent->childCount()) {
        q->beginRemoveRows(index(parent), row, parent->childCount() - 1);
        while (row < parent->childCount())
            delete parent->child(row);
        q->endRemoveRows();
    }
}

void SettingsModelPrivate::rebuild()
{
    Q_Q(SettingsModel);
    q->beginResetModel();
    fillGroup(rootItem);
    q->endResetModel();
}

void SettingsModelPrivate::submit(SettingsModelItem *item)
{
    settings->remove(item->key);
    if (item->type() != SettingsModelItem::Leaf) {
        settings->beginGroup(item->key);
        for (int i = 0; i < item->childCount(); i++) {
            submit(item->child(i));
        }
        settings->endGroup();
    } else {
        settings->setValue(item->key, item->value);
    }
}

SettingsModel::SettingsModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new SettingsModelPrivate(this))
{
    Q_D(SettingsModel);

    d->rootItem = new SettingsModelItem();
    d->settings = 0;
    d->editStrategy = OnFieldChange;
    d->readOnly = true;
    d->dirIcon.addPixmap(qApp->style()->standardPixmap(QStyle::SP_DirIcon));
    d->keyIcon.addPixmap(qApp->style()->standardPixmap(QStyle::SP_FileIcon));
}

SettingsModel::~SettingsModel()
{
    submitAll();

    delete d_ptr->rootItem;
    delete d_ptr;
}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const SettingsModel);

    if (!index.isValid())
        return QVariant();

    SettingsModelItem *item = static_cast<SettingsModelItem*>(index.internalPointer());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0 : return item->key;
        case 1 : return (item->type() != SettingsModelItem::Folder) ?
                        QVariant(QLatin1String(QVariant::typeToName(item->valueType))) :
                        QVariant();
        case 2 : return item->value;
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            if (item->type() == SettingsModelItem::Folder)
                return d->dirIcon;
            else
                return d->keyIcon;
        }
    } else if (role == Qt::ToolTipRole) {
        return item->longKey;
    }
    return QVariant();
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    SettingsModelItem *item = d_func()->item(index);
    SettingsModelItem::Type type = item->type();
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (type == SettingsModelItem::Leaf) {
        int column = index.column();
        if (column == 2 && !d_func()->readOnly)
            flags |= Qt::ItemIsEditable;
    }

    return flags;
}

bool SettingsModel::hasChildren(const QModelIndex &parent) const
{
    SettingsModelItem::Type type = d_func()->item(parent)->type();
    return type == SettingsModelItem::Folder || type == SettingsModelItem::Root;
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0 : return tr("Key");
        case 1 : return tr("Type");
        case 2 : return tr("Value");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex SettingsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SettingsModelItem *parentItem = d_func()->item(parent);
    SettingsModelItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex SettingsModel::parent(const QModelIndex &index) const
{
    Q_D(const SettingsModel);

    if (!index.isValid())
        return QModelIndex();

    SettingsModelItem *childItem = static_cast<SettingsModelItem*>(index.internalPointer());
    SettingsModelItem *parentItem = childItem->parent();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

bool SettingsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_D(SettingsModel);

    if (!d->settings)
        return false;

    if (row < 0 || count <= 0 || row + count > rowCount(parent))
        return false;

    if (d->readOnly)
        return false;

    SettingsModelItem *parentItem = d->item(parent);
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row + count - 1; i >= row; --i) {
        SettingsModelItem *childItem = parentItem->child(i);
        if (d->editStrategy == OnFieldChange) {
            d->settings->remove(childItem->longKey);
        } else {
            d->keysToRemove.append(childItem->longKey);
        }
        delete childItem;
    }
    endRemoveRows();

    return true;
}

int SettingsModel::rowCount(const QModelIndex &parent) const
{
    return d_func()->item(parent)->childCount();
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(SettingsModel);

    if (!d->settings)
        return false;

    if (!index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0)
        return false;

    if (d->readOnly)
        return false;

    SettingsModelItem *item = d->item(index);

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 2) {
            item->value = value;
            if (d->editStrategy == OnFieldChange) {
                d->settings->setValue(item->longKey, value);
            }
        }
    }
    emit dataChanged(index, index);
    return true;
}

QSettings *SettingsModel::settings() const
{
    return d_func()->settings;
}

void SettingsModel::setSettings(QSettings *settings)
{
    Q_D(SettingsModel);

    if (d->settings == settings)
        return;

    d->settings = settings;

    d->rebuild();
}

SettingsModel::EditStrategy SettingsModel::editStrategy() const
{
    return d_func()->editStrategy;
}

void SettingsModel::setEditStrategy(SettingsModel::EditStrategy strategy)
{
    Q_D(SettingsModel);

    if (d->editStrategy == strategy)
        return;

    d->editStrategy = strategy;
}

bool SettingsModel::readOnly() const
{
    return d_func()->readOnly;
}

void SettingsModel::setReadOnly(bool readOnly)
{
    d_func()->readOnly = readOnly;
}

void SettingsModel::clear()
{
    Q_D(SettingsModel);

    if (!d->settings)
        return;

    d->rebuild();
}

void SettingsModel::submitAll()
{
    Q_D(SettingsModel);

    if (d->editStrategy == OnFieldChange)
        return;

    if (!d->settings)
        return;

    foreach (const QString &key, d->keysToRemove) {
        d->settings->remove(key);
    }

    d->submit(d->rootItem);
}

void SettingsModel::revertAll()
{
    Q_D(SettingsModel);

    if (d->editStrategy == OnFieldChange)
        return;

    d->keysToRemove.clear();
    refresh();
}

void SettingsModel::refresh()
{
    Q_D(SettingsModel);

    d->refresh(d->rootItem);
}
