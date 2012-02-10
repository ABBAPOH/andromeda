#include "commandsmodel.h"
#include "commandsmodel_p.h"

#include "actionmanager.h"
#include "commandcontainer.h"

#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QFont>

using namespace GuiSystem;

QModelIndex CommandsModelPrivate::index(TreeItem *item) const
{
    return q_func()->createIndex(item->row(), 0, item);
}

TreeItem * CommandsModelPrivate::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return rootItem;

    return static_cast<TreeItem*>(index.internalPointer());
}

void CommandsModelPrivate::build()
{
    Q_Q(CommandsModel);

    q->beginResetModel();

    qDeleteAll(rootItem->children());
    mapToItem.clear();

    ActionManager *am = ActionManager::instance();

    foreach (CommandContainer *container, am->containers()) {
        QList<Command *> commands = container->commands();
        if (commands.isEmpty())
            continue;

        TreeItem *categoryItem = new TreeItem(TreeItem::Folder, rootItem);
        categoryItem->name = container->title();

        foreach (Command *c, commands) {
            if (c->hasAttribute(Command::AttributeNonConfigurable))
                continue;

            TreeItem *item = new TreeItem(TreeItem::Leaf, categoryItem);
            item->cmd = c;
            mapToItem.insert(c->shortcut(), item);
        }
    }

    q->endResetModel();
}

CommandsModel::CommandsModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new CommandsModelPrivate(this))
{
    Q_D(CommandsModel);

    d->rootItem = new TreeItem();
    d->settings = new QSettings(this);
    d->settings->beginGroup(QLatin1String("ActionManager/Shortcuts"));

    d->build();
}

CommandsModel::~CommandsModel()
{
  delete d_ptr->rootItem;
  delete d_ptr;
}

int CommandsModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant CommandsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    int column = index.column();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (item->type() == TreeItem::Leaf) {
            switch (column) {
            case 0: return item->cmd->defaultText();
            case 1: return item->cmd->shortcut();
            }
        } else {
            switch (column) {
            case 0: return item->name;
            }
        }
    } else if (role == Qt::DecorationRole) {

    } else if (role == Qt::ToolTipRole) {

    } else if (role == Qt::FontRole) {
        QFont f = qApp->font();
        if (item->type() == TreeItem::Folder) {
            f.setBold(true);
            return f;
        } else {
            if (column == 1 && isModified(index)) {
                f.setBold(true);
                return f;
            }
        }
    } else if (role == Qt::TextColorRole) {
        if (item->type() == TreeItem::Leaf) {
            if (column == 1 && d_func()->mapToItem.values(item->cmd->shortcut()).count() > 1) {
                return Qt::red;
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags CommandsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool CommandsModel::hasChildren(const QModelIndex &parent) const
{
    TreeItem::Type type = d_func()->item(parent)->type();
    return type == TreeItem::Folder || type == TreeItem::Root;
}

QVariant CommandsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0 : return tr("Command");
        case 1 : return tr("Shortcut");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex CommandsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = d_func()->item(parent);
    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex CommandsModel::parent(const QModelIndex &index) const
{
    Q_D(const CommandsModel);

    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CommandsModel::rowCount(const QModelIndex &parent) const
{
    return d_func()->item(parent)->childCount();
}

bool CommandsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(CommandsModel);

    if (!index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0)
        return false;

    TreeItem *item = d->item(index);

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (item->type() == TreeItem::Leaf) {
            QKeySequence oldShortcut = item->cmd->shortcut();
            QString shortcut = value.toString();
            d->mapToItem.remove(oldShortcut, item);
            item->cmd->setShortcut(shortcut);
            d->mapToItem.insert(item->cmd->shortcut(), item);
            d->settings->setValue(item->cmd->id(), shortcut);

            foreach (TreeItem *item, d->mapToItem.values(oldShortcut)) {
                QModelIndex index = d->index(item);
                index = index.sibling(index.row(), 1);
                emit dataChanged(index, index);
            }
        }
        break;
    default:
        break;
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

bool CommandsModel::isModified(const QModelIndex &index) const
{
    Q_D(const CommandsModel);

    TreeItem *item = d->item(index);
    if (!item->cmd)
        return false;

    return item->cmd->shortcut() != item->cmd->defaultShortcut();
}

void CommandsModel::resetShortcut(const QModelIndex &index)
{
    Q_D(CommandsModel);

    TreeItem *item = d->item(index);
    if (item->type() == TreeItem::Leaf) {
        Command *c = item->cmd;
        if (c->shortcut() != c->defaultShortcut()) {
            QKeySequence oldShortcut = item->cmd->shortcut();

            d->mapToItem.remove(oldShortcut, item);
            c->setShortcut(c->defaultShortcut());
            d->mapToItem.insert(item->cmd->shortcut(), item);

            foreach (TreeItem *item, d->mapToItem.values(oldShortcut)) {
                QModelIndex index = d->index(item);
                index = index.sibling(index.row(), 1);
                emit dataChanged(index, index);
            }

            d->settings->remove(c->id());
            QModelIndex sib = sibling(index.row(), 1, index);
            emit dataChanged(sib, sib);
        }
    }
}

bool CommandsModel::exportShortcuts(QIODevice *device) const
{
    return ActionManager::instance()->exportShortcuts(device);
}

bool CommandsModel::importShortcuts(QIODevice *device)
{
    bool ok = ActionManager::instance()->importShortcuts(device);
    d_func()->build();
    return ok;
}
