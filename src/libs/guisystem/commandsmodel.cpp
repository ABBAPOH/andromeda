#include "commandsmodel.h"
#include "commandsmodel_p.h"

#include "actionmanager.h"
#include "commandcontainer.h"

#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QFont>

using namespace GuiSystem;

QModelIndex CommandsModelPrivate::index(CommandsModelItem *item) const
{
    return q_func()->createIndex(item->row(), 0, item);
}

CommandsModelItem * CommandsModelPrivate::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return rootItem;

    return static_cast<CommandsModelItem*>(index.internalPointer());
}

void CommandsModelPrivate::setItemShortcut(CommandsModelItem *item, const QKeySequence &shortcut)
{
    QKeySequence oldShortcut = item->cmd->shortcut();

    mapToCommand.remove(oldShortcut, item->cmd);
    mapToItem.remove(oldShortcut, item);
    item->cmd->setShortcut(shortcut);
    mapToCommand.insert(item->cmd->shortcut(), item->cmd);
    mapToItem.insert(item->cmd->shortcut(), item);
}

void CommandsModelPrivate::build()
{
    Q_Q(CommandsModel);

    q->beginResetModel();

    qDeleteAll(rootItem->children());
    mapToItem.clear();
    mapToCommand.clear();

    ActionManager *am = ActionManager::instance();

    foreach (CommandContainer *container, am->containers()) {
        QList<Command *> commands = container->commands();
        if (commands.isEmpty())
            continue;

        CommandsModelItem *categoryItem = new CommandsModelItem(CommandsModelItem::Folder, rootItem);
        categoryItem->name = container->title();

        foreach (Command *c, commands) {
            if (c->isSeparator())
                continue;

            CommandsModelItem *item = new CommandsModelItem(CommandsModelItem::Leaf, categoryItem);
            item->cmd = c;
            if (!mapToCommand.values(c->shortcut()).contains(c))
                mapToCommand.insert(c->shortcut(), c);
            mapToItem.insert(c->shortcut(), item);
        }
    }

    q->endResetModel();
}

/*!
    \class GuiSystem::CommandsModel

    \brief CommandsModel is a model used to display available Commands.

    CommandsModel shows available Commands as tree, grouping Commands by their
    container. Also this model allows to change Commands' shortcuts.
*/

/*!
    Creates CommandsModel with the given \a parent.
*/
CommandsModel::CommandsModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new CommandsModelPrivate(this))
{
    Q_D(CommandsModel);

    d->rootItem = new CommandsModelItem();
    d->settings = new QSettings(this);
    d->settings->beginGroup(QLatin1String("ActionManager/Shortcuts"));

    d->build();
}

/*!
    Destroys CommandsModel.
*/
CommandsModel::~CommandsModel()
{
  delete d_ptr->rootItem;
  delete d_ptr;
}

/*!
    \reimp
*/
int CommandsModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

/*!
    \reimp
*/
QVariant CommandsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    CommandsModelItem *item = static_cast<CommandsModelItem*>(index.internalPointer());
    int column = index.column();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (item->type() == CommandsModelItem::Leaf) {
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
        if (item->type() == CommandsModelItem::Folder) {
            f.setBold(true);
            return f;
        } else {
            if (column == 1 && isModified(index)) {
                f.setBold(true);
                return f;
            }
        }
    } else if (role == Qt::TextColorRole) {
        if (item->type() == CommandsModelItem::Leaf) {
            if (column == 1 && d_func()->mapToCommand.values(item->cmd->shortcut()).count() > 1) {
                return Qt::red;
            }
        }
    }
    return QVariant();
}

/*!
    \reimp
*/
Qt::ItemFlags CommandsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.parent().isValid()) {
        CommandsModelItem *item = d_func()->item(index);
        if (item->cmd->hasAttribute(Command::AttributeNonConfigurable))
            return Qt::NoItemFlags;

        if (index.column() == 1)
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/*!
    \reimp
*/
bool CommandsModel::hasChildren(const QModelIndex &parent) const
{
    CommandsModelItem::Type type = d_func()->item(parent)->type();
    return type == CommandsModelItem::Folder || type == CommandsModelItem::Root;
}

/*!
    \reimp
*/
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

/*!
    \reimp
*/
QModelIndex CommandsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CommandsModelItem *parentItem = d_func()->item(parent);
    CommandsModelItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

/*!
    \reimp
*/
QModelIndex CommandsModel::parent(const QModelIndex &index) const
{
    Q_D(const CommandsModel);

    if (!index.isValid())
        return QModelIndex();

    CommandsModelItem *childItem = static_cast<CommandsModelItem*>(index.internalPointer());
    CommandsModelItem *parentItem = childItem->parent();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

/*!
    \reimp
*/
int CommandsModel::rowCount(const QModelIndex &parent) const
{
    return d_func()->item(parent)->childCount();
}

/*!
    \reimp
*/
bool CommandsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(CommandsModel);

    if (!index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0)
        return false;

    CommandsModelItem *item = d->item(index);

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (item->type() == CommandsModelItem::Leaf) {
            QKeySequence oldShortcut = item->cmd->shortcut();
            QString shortcut = value.toString();
            d->setItemShortcut(item, value.toString());
            d->settings->setValue(item->cmd->id(), shortcut);

            foreach (CommandsModelItem *item, d->mapToItem.values(oldShortcut)) {
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

/*!
    Returns true if Command's shortcut differs from the default one.
*/
bool CommandsModel::isModified(const QModelIndex &index) const
{
    Q_D(const CommandsModel);

    CommandsModelItem *item = d->item(index);
    if (!item->cmd)
        return false;

    return item->cmd->shortcut() != item->cmd->defaultShortcut();
}

/*!
    Resets Command's shortcut to the default one.
*/
void CommandsModel::resetShortcut(const QModelIndex &index)
{
    Q_D(CommandsModel);

    CommandsModelItem *item = d->item(index);
    if (item->type() == CommandsModelItem::Leaf) {
        Command *c = item->cmd;
        if (c->shortcut() != c->defaultShortcut()) {
            QKeySequence oldShortcut = item->cmd->shortcut();

            d->setItemShortcut(item, c->defaultShortcut());

            foreach (CommandsModelItem *item, d->mapToItem.values(oldShortcut)) {
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

/*!
    Exports all shortcuts to a Keyboard mappings scheme (*.kms) file.
*/
bool CommandsModel::exportShortcuts(QIODevice *device) const
{
    return ActionManager::instance()->exportShortcuts(device);
}

/*!
    Imports all shortcuts from a Keyboard mappings scheme (*.kms) file.
*/
bool CommandsModel::importShortcuts(QIODevice *device)
{
    bool ok = ActionManager::instance()->importShortcuts(device);
    d_func()->build();
    return ok;
}
