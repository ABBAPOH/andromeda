#include "pluginviewmodel.h"
#include "pluginviewmodel_p.h"

#include <QDebug>

using namespace ExtensionSystem;

// ============= PluginViewModel =============

PluginViewModel::PluginViewModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(new PluginViewModelPrivate)
{
    Q_D(PluginViewModel);
    connect(d->manager, SIGNAL(pluginsChanged()), this, SLOT(updateModel()));
    foreach (PluginSpec *spec, d->manager->plugins()) {
        d->node(spec);
    }
}

PluginViewModel::~PluginViewModel()
{
    delete d_ptr;
}

int PluginViewModel::columnCount(const QModelIndex &parent) const
{
    return 12;
}

QVariant PluginViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::CheckStateRole) {
        Node *node = static_cast<Node*>(index.internalPointer());
        if (!node->isCategory) {
            if (index.column() == 1) {
                return node->spec->loadOnStartup() ? 2 : 0;
            }
            if (index.column() == 2) {
                return node->spec->enabled() ? 2 : 0;
            }
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {

        Node *node = static_cast<Node*>(index.internalPointer());
        if (node->isCategory) {
            if (index.column() == 0) {
                return node->categoryName;
            }
        } else {
            switch (index.column()) {
            case 0: return node->spec->name();
//            case 1: return node->spec->loadsOnStartup();
//            case 2: return node->spec->enabled();
            case 3: return node->spec->version();
            case 4: return node->spec->compatibilityVersion();
            case 5: return node->spec->vendor();
            case 6: return node->spec->url();
            case 7: return node->spec->libraryPath();
            case 8: return node->spec->description();
            case 9: return node->spec->copyright();
            case 10: return node->spec->license();
            case 11: QString result;
                foreach (PluginDependency dep, node->spec->dependencies()) {
                    result += dep.name() + ' ' + '(' + dep.version() + ") \n";
                }
                return result.mid(0, result.length()-2);
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags PluginViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == 1 || index.column() == 2) {
        flags |= /*Qt::ItemIsEditable |*/ Qt::ItemIsUserCheckable;
    }
    return flags;
}

QVariant PluginViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("Name");
        case 1: return tr("Loads on Startup");
        case 2: return tr("Enabled");
        case 3: return tr("Version");
        case 4: return tr("Compatibility Version");
        case 5: return tr("Vendor");
        case 6: return tr("Url");
        case 7: return tr("Library Path");
        case 8: return tr("Description");
        case 9: return tr("Copyright");
        case 10: return tr("license");
        case 11: return tr("Dependencies");
        }
    }

    return QVariant();
}

QModelIndex PluginViewModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    Node *parentNode;

    if (!parent.isValid())
        parentNode = d_func()->rootNode;
    else
        parentNode = static_cast<Node *>(parent.internalPointer());

    Node *childNode = parentNode->children.at(row);
    if (childNode)
        return createIndex(row, column, childNode);
    else
        return QModelIndex();
}

QModelIndex PluginViewModel::parent(const QModelIndex &index) const
{
//    qDebug("PluginViewModel::parent");
//    qDebug() << index;
    if (!index.isValid())
        return QModelIndex();

    Node *childNode = static_cast<Node *>(index.internalPointer());
    Node *parentNode = childNode->parent;
    if (parentNode == d_func()->rootNode)
        return QModelIndex();

//    qDebug() << "create index" << createIndex(parentNode->row(), 0, parentNode);
    return createIndex(parentNode->row(), 0, parentNode);
}

int PluginViewModel::rowCount(const QModelIndex &parent) const
{
    Node *parentNode;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = d_func()->rootNode;
    else
        parentNode = static_cast<Node *>(parent.internalPointer());

    return parentNode->childCount();
}

bool PluginViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return true;
    if (role == Qt::CheckStateRole) {
        if (index.column() == 1) {
            Node *node = static_cast<Node *>(index.internalPointer());
            // in fact we receive 0, 1 or 2 but 0 is false and 2 is true so everything ok
            node->spec->setLoadOnStartup(value.toBool());
            return true;
        }
        if (index.column() == 2) {
            Node *node = static_cast<Node *>(index.internalPointer());
            node->spec->setEnabled(value.toBool());
            qDebug() << "enabled:" << node->spec->enabled();
            qDebug() << node->spec->errorString();
            return !node->spec->enabled();
        }
    }
    return false;
}

void PluginViewModel::updateModel()
{
    Q_D(PluginViewModel);

    foreach (PluginSpec *spec, d->manager->plugins()) {
        if (!d->nodesForCategories.contains(spec->category())) {
            const QString &category = spec->category();
            int rows = rowCount();
            beginInsertRows(QModelIndex(), rows, rows);
            d->node(category);
            endInsertRows();
        }
        if (!d->nodesForSpecs.contains(spec)) {
            QModelIndex categoryIndex = index(d->node(spec->category())->row(), 0);
            int rows = rowCount(categoryIndex);
            beginInsertRows(categoryIndex, rows, rows);
            d->node(spec);
            endInsertRows();
        }
    }
//    this->reset();
}

// ============= PluginViewModelPrivate =============

PluginViewModelPrivate::PluginViewModelPrivate()
{
    manager = PluginManager::instance();
    rootNode = new Node;
}

PluginViewModelPrivate::~PluginViewModelPrivate()
{
    delete rootNode;
}

Node *PluginViewModelPrivate::node(const QString &category)
{
    qDebug() << category;
    if (nodesForCategories.contains(category))
        return nodesForCategories.value(category);
    Node *result = new Node(rootNode);
    result->categoryName = category;
    result->isCategory = true;
    nodesForCategories.insert(category, result);
    return result;
}

Node * PluginViewModelPrivate::node(PluginSpec *spec)
{
    if (nodesForSpecs.contains(spec))
        return nodesForSpecs.value(spec);
    Node *parent = node(spec->category());
    Node *result = new Node(parent);
    result->spec = spec;
    nodesForSpecs.insert(spec, result);
    return result;
}

