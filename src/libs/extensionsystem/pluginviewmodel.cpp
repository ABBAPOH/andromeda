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
    return 4;
}

QVariant PluginViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {

        Node *node = static_cast<Node*>(index.internalPointer());
        switch (index.column()) {
        case 0: if (node->isCategory) return node->categoryName; else return node->spec->name();
        case 1: if (node->isCategory) return QVariant(); else return node->spec->version();
        case 2: if (node->isCategory) return QVariant(); else return node->spec->compatibilityVersion();
        case 3: if (node->isCategory) return QVariant(); else return node->spec->vendor();
        }
    }
    return QVariant();
}

Qt::ItemFlags PluginViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PluginViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("Name");
        case 1: return tr("Version");
        case 2: return tr("Compatibility Version");
        case 3: return tr("Vendor");
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

