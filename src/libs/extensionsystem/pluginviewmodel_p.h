#ifndef PLUGINVIEWMODEL_P_H
#define PLUGINVIEWMODEL_P_H

#include "pluginmanager.h"

namespace ExtensionSystem {

struct Node
{
    Node() :
            parent(0),
            m_row(0),
            spec(0),
            isCategory(false)
    {
    }

    Node(Node * parent) :
            spec(0),
            isCategory(false)
    {
        this->parent = parent;
        m_row = parent->children.size();
        parent->children.append(this);
    }

    ~Node()
    {
        if (parent)
            parent->children.removeAll(this);

        // Copy list to prevent modifying it while deleting children
        QList<Node *> children = this->children;
        qDeleteAll(children);
    }

    int row() { return m_row; }
    int childCount() { return children.size(); }
    Node * parent;
    QList<Node *> children;
    int m_row;
    // data
    PluginSpec * spec;
    bool isCategory;
    QString categoryName;
};

class PluginViewModelPrivate
{
public:
    PluginViewModelPrivate();
    ~PluginViewModelPrivate();

    PluginManager * manager;
    Node * rootNode;
    QHash<QString, Node *> nodesForCategories;
    QHash<PluginSpec*, Node *> nodesForSpecs;

    Node * node(const QString &category);
    Node * node(PluginSpec *spec);
};

} // namespace ExtensionSystem

#endif // PLUGINVIEWMODEL_P_H
