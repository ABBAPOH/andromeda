#include "navigationpanel.h"

#include "navigationmodel.h"

#include <QtGui/QTreeView>
#include <QtGui/QResizeEvent>

using namespace FileManagerPlugin;

NavigationPanel::NavigationPanel(QWidget *parent) :
    QWidget(parent),
    m_treeView(new QTreeView(this)),
    m_model(new NavigationModel(this))
{
    m_treeView->setModel(m_model);
    m_treeView->setHeaderHidden(true);
    m_treeView->setFocusPolicy(Qt::NoFocus);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDragEnabled(true);
    m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
//    m_treeView->setSelectionMode(QAbstractItemView::NoSelection);

    QPalette pal = m_treeView->palette();
    pal.setColor(QPalette::Base, QColor(214, 221, 229));
    m_treeView->QAbstractItemView::setPalette(pal);
    m_treeView->expandAll();

    connect(m_treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClick(QModelIndex)));
    connect(m_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

    setMinimumSize(200, 200);
}

NavigationPanel::~NavigationPanel()
{
}

void NavigationPanel::addFolder(const QString & path)
{
    m_model->addFolder(path);
}

void NavigationPanel::removeFolder(const QString &path)
{
    m_model->removeFolder(path);
}

QString NavigationPanel::currentPath() const
{
    return m_currentPath;
}

void NavigationPanel::resizeEvent(QResizeEvent * event)
{
    m_treeView->resize(event->size());
    QWidget::resizeEvent(event);
}

void NavigationPanel::onClick(const QModelIndex &index)
{
    QString path = m_model->path(index);
    if (!path.isEmpty()) {
        m_currentPath  = path;
        emit currentPathChanged(path);
    } else {
        m_treeView->selectionModel()->select(m_model->index(m_currentPath), QItemSelectionModel::Select);
    }
}
