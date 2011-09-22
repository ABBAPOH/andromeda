#include "navigationpanel.h"

#include "navigationmodel.h"

#include <QtGui/QAction>
#include <QtGui/QMenu>
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

    QAction *openAction;
    QAction *removeAction;
    QMenu *contextMenu;

    QModelIndex selectedRow() const;
};

} // namespace FileManagerPlugin

using namespace FileManagerPlugin;

QModelIndex NavigationPanelPrivate::selectedRow() const
{
    QModelIndexList indexes = treeView->selectionModel()->selectedRows();
    if (indexes.isEmpty())
        return QModelIndex();

    return indexes.first();
}

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
    d->treeView->setDragDropMode(QAbstractItemView::DropOnly);

    QPalette pal = d->treeView->palette();
    pal.setColor(QPalette::Base, QColor(214, 221, 229));
    d->treeView->QAbstractItemView::setPalette(pal);
    d->treeView->expandAll();

    connect(d->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClick(QModelIndex)));

    d->contextMenu = new QMenu(this);
    d->openAction = d->contextMenu->addAction(tr("Open"), this, SLOT(onOpenTriggered()));
    d->contextMenu->addSeparator();
    d->removeAction = d->contextMenu->addAction(tr("Remove from panel"), this, SLOT(onRemoveTriggered()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);

    setMinimumSize(100, 200);
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

NavigationModel * NavigationPanel::model() const
{
    return d_func()->model;
}

void NavigationPanel::setModel(NavigationModel *model)
{
    Q_D(NavigationPanel);

    if (d->model && d->model->QObject::parent() == this)
        d->model->deleteLater();

    if (d->model != model) {
        d->model = model;
        d->treeView->setModel(model);
        d->treeView->expandAll();
    }
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
        emit currentPathChanged(path);
        emit triggered(path);
    } else {
        d->treeView->selectionModel()->select(d->model->index(d->currentPath), QItemSelectionModel::Select);
    }
}

void NavigationPanel::onCustomContextMenuRequested(QPoint p)
{
    Q_D(NavigationPanel);

    QModelIndex index = d->selectedRow();
    if (!index.isValid())
        return;

    // fixme : awful code
    d->removeAction->setEnabled(index.parent().row()==2);
    d->contextMenu->exec(mapToGlobal(p));
}

void NavigationPanel::onOpenTriggered()
{
    Q_D(NavigationPanel);

    emit triggered(d->model->path(d->selectedRow()));
}

void NavigationPanel::onRemoveTriggered()
{
    Q_D(NavigationPanel);

    d->model->removeFolder(d->model->path(d->selectedRow()));
}
