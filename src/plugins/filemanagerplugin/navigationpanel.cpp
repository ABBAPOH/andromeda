#include "navigationpanel.h"

#include "navigationmodel.h"

#include <QtGui/QTreeView>
#include <QtGui/QResizeEvent>

#define bublic public

namespace FileManagerPlugin {

class NavigationPanelPrivate
{
bublic:
    QTreeView *treeView;
    NavigationModel *model;
    QString currentPath;
};

} // namespace FileManagerPlugin

using namespace FileManagerPlugin;

NavigationPanel::NavigationPanel(QWidget *parent) :
    QWidget(parent),
    d_ptr(new NavigationPanelPrivate)
{
    Q_D(NavigationPanel);

    d->model = new NavigationModel(this);
    d->treeView = new QTreeView(this);

    d->treeView->setModel(d->model);
    d->treeView->setHeaderHidden(true);
    d->treeView->setFocusPolicy(Qt::NoFocus);
    d->treeView->setAcceptDrops(true);
    d->treeView->setDropIndicatorShown(true);
    d->treeView->setDragEnabled(true);
    d->treeView->setDragDropMode(QAbstractItemView::DragDrop);

    QPalette pal = d->treeView->palette();
    pal.setColor(QPalette::Base, QColor(214, 221, 229));
    d->treeView->QAbstractItemView::setPalette(pal);
    d->treeView->expandAll();

    connect(d->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClick(QModelIndex)));

    setMinimumSize(200, 200);
}

NavigationPanel::~NavigationPanel()
{
    delete d_ptr;
}

void NavigationPanel::addFolder(const QString & path)
{
    Q_D(NavigationPanel);

    d->model->addFolder(path);
}

void NavigationPanel::removeFolder(const QString &path)
{
    Q_D(NavigationPanel);

    d->model->removeFolder(path);
}

QString NavigationPanel::currentPath() const
{
    return d_func()->currentPath;
}

void NavigationPanel::resizeEvent(QResizeEvent * event)
{
    Q_D(NavigationPanel);

    d->treeView->resize(event->size());
    QWidget::resizeEvent(event);
}

void NavigationPanel::onClick(const QModelIndex &index)
{
    Q_D(NavigationPanel);

    QString path = d->model->path(index);
    if (!path.isEmpty()) {
        d->currentPath  = path;
        qDebug("emit");
        emit currentPathChanged(path);
    } else {
        d->treeView->selectionModel()->select(d->model->index(d->currentPath), QItemSelectionModel::Select);
    }
}
