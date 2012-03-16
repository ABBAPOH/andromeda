#include "bookmarkswidget.h"
#include "bookmarkswidget_p.h"

using namespace Bookmarks;

BookmarksWidget::BookmarksWidget(QWidget *parent) :
    QWidget(parent),
    d(new BookmarksWidgetPrivate)
{
    setupUi();

    d->model = 0;
    d->folderProxy = new FolderProxyModel(this);
    d->proxyModel = new BookmarkListFilterModel(this);
    d->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    d->treeView->setModel(d->folderProxy);
    d->tableView->setModel(d->proxyModel);

    connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), d->tableView, SLOT(clearSelection()));
    connect(d->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));
    connect(d->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showTableViewMenu(QPoint)));
    connect(d->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClicked(QModelIndex)));
    connect(d->treeView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showTreeViewMenu(QPoint)));
    connect(d->lineEdit, SIGNAL(textEdited(QString)), SLOT(onTextEdited(QString)));

    connect(d->addFolderAction, SIGNAL(triggered()), SLOT(addFolder()));
    connect(d->openAction, SIGNAL(triggered()), SLOT(openTriggered()));
    connect(d->openInTabAction, SIGNAL(triggered()), SLOT(openInTabTriggered()));
    connect(d->openInWindowAction, SIGNAL(triggered()), SLOT(openInWindowTriggered()));
    connect(d->openInTabsAction, SIGNAL(triggered()), SLOT(openInTabsTriggered()));

    connect(d->editUrl, SIGNAL(triggered()), SLOT(editUrl()));
    connect(d->editDescription, SIGNAL(triggered()), SLOT(editDescription()));
    connect(d->renameAction, SIGNAL(triggered()), SLOT(rename()));

    connect(d->removeAction, SIGNAL(triggered()), SLOT(remove()));

    connect(d->splitter, SIGNAL(splitterMoved(int,int)), SIGNAL(stateChanged()));
}

BookmarksWidget::~BookmarksWidget()
{
    delete d;
}

BookmarksModel * BookmarksWidget::model() const
{
    return d->model;
}

void BookmarksWidget::setModel(BookmarksModel *model)
{
    d->model = model;
    d->folderProxy->setSourceModel(model);
    d->proxyModel->setSourceModel(model);
    d->treeView->expandAll();
    d->tableView->setColumnWidth(0, 200);
    d->tableView->setColumnWidth(1, 200);
    d->treeView->selectionModel()->clearSelection();
    QModelIndex index = model->index(0,0, QModelIndex());
    index = d->folderProxy->mapFromSource(index);
    d->treeView->selectionModel()->select(index, QItemSelectionModel::Select);
    onClicked(index);
}

bool BookmarksWidget::restoreState(const QByteArray &state)
{
    return d->splitter->restoreState(state);
}

QByteArray BookmarksWidget::saveState() const
{
    return d->splitter->saveState();
}

QSize BookmarksWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize BookmarksWidget::sizeHint() const
{
    return QSize(800, 600);
}

void BookmarksWidget::onClicked(const QModelIndex &index)
{
    QModelIndex sourceIndex = d->folderProxy->mapToSource(index);
    QModelIndex proxyIndex = d->proxyModel->mapFromSource(sourceIndex);
    d->proxyModel->setRootIndex(sourceIndex);
    d->tableView->setRootIndex(proxyIndex);
}

void BookmarksWidget::onDoubleClicked(const QModelIndex &index)
{
    QModelIndex sourceIndex = d->proxyModel->mapToSource(index);
    QModelIndex folderIndex = d->folderProxy->mapFromSource(sourceIndex);
    if (d->model->isFolder(sourceIndex)) {
        d->proxyModel->setRootIndex(sourceIndex);
        d->treeView->selectionModel()->clear();
        d->treeView->selectionModel()->select(folderIndex, QItemSelectionModel::Select);
        d->tableView->setRootIndex(index);
    } else {
        emit open(d->model->data(sourceIndex, BookmarksModel::UrlRole).toUrl());
    }
}

void BookmarksWidget::onTextEdited(const QString &text)
{
    d->proxyModel->setFilterFixedString(text);
}

void BookmarksWidget::addFolder()
{
    QWidget *w = focusWidget();
    if (w == d->treeView) {
        QModelIndexList indexes = d->treeView->selectionModel()->selectedIndexes();
        if (indexes.isEmpty())
            return;
        QModelIndex index = indexes.first();
        QModelIndex sourceIndex = d->folderProxy->mapToSource(index);
        QModelIndex newIndex = d->model->addFolder("New bookmark folder", sourceIndex);
        d->treeView->expand(index);
        d->treeView->edit(d->folderProxy->mapFromSource(newIndex));
    } else {
        QModelIndex index = d->tableView->rootIndex();
        QModelIndex sourceIndex = d->proxyModel->mapToSource(index);
        QModelIndex newIndex = d->model->addFolder("New bookmark folder", sourceIndex);
        d->tableView->edit(d->proxyModel->mapFromSource(newIndex));
    }
}

void BookmarksWidget::showTreeViewMenu(QPoint p)
{
    QMenu menu;
    menu.addAction(d->openInTabsAction);
    menu.addSeparator();
    menu.addAction(d->renameAction);
    menu.addSeparator();
    menu.addAction(d->removeAction);
    menu.exec(d->treeView->mapToGlobal(p));
}

void BookmarksWidget::showTableViewMenu(QPoint p)
{
    QWidget *w = focusWidget();
    if (w != d->tableView)
        return;

    QModelIndex sourceIndex = selectedIndex();
    QMenu menu;

    if (!sourceIndex.isValid()) {
        menu.addAction(d->addFolderAction);
    } else if (d->model->isFolder(sourceIndex)) {
        menu.addAction(d->openInTabsAction);
        menu.addSeparator();
        menu.addAction(d->renameAction);
        menu.addSeparator();
        menu.addAction(d->removeAction);
    } else {
        menu.addAction(d->openAction);
        menu.addAction(d->openInTabAction);
        menu.addAction(d->openInWindowAction);
        menu.addSeparator();
        menu.addAction(d->renameAction);
        menu.addAction(d->editUrl);
        menu.addAction(d->editDescription);
        menu.addSeparator();
        menu.addAction(d->removeAction);
    }
    menu.exec(d->tableView->mapToGlobal(p));
}

void BookmarksWidget::openTriggered()
{
    QModelIndex index = selectedBookmarkIndex();
    if (index.isValid())
        emit open(d->model->data(index, BookmarksModel::UrlRole).toUrl());
}

void BookmarksWidget::openInTabTriggered()
{
    QModelIndex index = selectedBookmarkIndex();
    if (index.isValid())
        emit openInTab(d->model->data(index, BookmarksModel::UrlRole).toUrl());
}

void BookmarksWidget::openInWindowTriggered()
{
    QModelIndex index = selectedBookmarkIndex();
    if (index.isValid())
        emit openInWindow(d->model->data(index, BookmarksModel::UrlRole).toUrl());
}

void BookmarksWidget::openInTabsTriggered()
{
    QList<QUrl> urls;
    QModelIndex index = selectedIndex();
    if (!index.isValid())
        return;
    QModelIndexList indexes;
    indexes.append(index);
    while (!indexes.isEmpty()) {
        index = indexes.takeFirst();
        for (int i = 0; i < d->model->rowCount(index); i++) {
            QModelIndex idx = d->model->index(i, 0, index);
            if (d->model->isFolder(idx)) {
                indexes.append(idx);
            } else {
                urls.append(d->model->data(idx, BookmarksModel::UrlRole).toUrl());
            }
        }
    }
    emit open(urls);
}

void BookmarksWidget::rename()
{
    QWidget *w = focusWidget();
    QAbstractItemView *view = qobject_cast<QAbstractItemView*>(w);
    if (view) {
        QModelIndexList indexes = view->selectionModel()->selectedIndexes();
        if (indexes.isEmpty())
            return;
        QModelIndex index = indexes.first();
        view->edit(index);
    }
}

void BookmarksWidget::editUrl()
{
    QModelIndex index = selectedBookmarkIndex();
    if (index.isValid()) {
        index = d->proxyModel->mapFromSource(index);
        index = d->proxyModel->index(index.row(), 1, index.parent());
        d->tableView->edit(index);
    }
}

void BookmarksWidget::editDescription()
{
    QModelIndex index = selectedBookmarkIndex();
    if (index.isValid()) {
        index = d->proxyModel->mapFromSource(index);
        index = d->proxyModel->index(index.row(), 2, index.parent());
        d->tableView->edit(index);
    }
}

void BookmarksWidget::remove()
{
    QModelIndex index = selectedIndex();
    if (index.isValid())
        d->model->remove(index);
}

void BookmarksWidget::setupUi()
{
    d->toolBar = new QToolBar();

    d->addFolderAction = new QAction(tr("Add folder"), d->toolBar);

    d->spacer = new QWidget(d->toolBar);
    d->spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    d->lineEdit = new QLineEdit(d->toolBar);
    d->lineEdit->setPlaceholderText(tr("Filter"));
    d->lineEdit->setStyleSheet("QLineEdit { border-radius : 8px; }");
    d->lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->lineEdit->setMinimumWidth(200);

    d->toolBar->addAction(d->addFolderAction);
    d->toolBar->addWidget(d->spacer);
    d->toolBar->addWidget(d->lineEdit);

    d->splitter = new QSplitter(this);
    d->splitter->setHandleWidth(1);

    d->treeView = new QTreeView(d->splitter);
    d->treeView->setHeaderHidden(true);
    d->treeView->setExpandsOnDoubleClick(true);
    d->treeView->setEditTriggers(QAbstractItemView::SelectedClicked);
    d->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    d->treeView->setDragDropMode(QAbstractItemView::DragDrop);
    d->treeView->setAcceptDrops(true);
    d->treeView->setDefaultDropAction(Qt::MoveAction);

    d->tableView = new QTreeView(d->splitter);
    d->tableView->setItemsExpandable(false);
    d->tableView->setRootIsDecorated(false);
    d->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    d->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    d->tableView->setDragDropMode(QAbstractItemView::DragDrop);
    d->tableView->setAcceptDrops(true);
    d->tableView->setDefaultDropAction(Qt::MoveAction);

    d->splitter->setSizes(QList<int>() << 300 << 900); // 1200; 1/4

    d->mainLayout = new QVBoxLayout(this);
    d->mainLayout->setContentsMargins(0, 0, 0, 0);
    d->mainLayout->setSpacing(0);
    d->mainLayout->addWidget(d->toolBar);
    d->mainLayout->addWidget(d->splitter);
    setLayout(d->mainLayout);

    d->openAction = new QAction(tr("Open"), this);
    d->openInTabAction = new QAction(tr("Open in new tab"), this);
    d->openInWindowAction = new QAction(tr("Open in new window"), this);
    d->openInTabsAction = new QAction(tr("Open in all tabs"), this);
    d->editUrl = new QAction(tr("Edit url"), this);
    d->editDescription = new QAction(tr("Edit description"), this);
    d->renameAction = new QAction(tr("Rename"), this);
    d->removeAction = new QAction(tr("Remove"), this);
}

QModelIndex BookmarksWidget::selectedIndex()
{
    QWidget *w = focusWidget();
    if (w == d->tableView) {
        QModelIndexList indexes = d->tableView->selectionModel()->selectedIndexes();
        if (!indexes.isEmpty())
            return d->proxyModel->mapToSource(indexes.first());
    } else if (w == d->treeView) {
        QModelIndexList indexes = d->treeView->selectionModel()->selectedIndexes();
        if (!indexes.isEmpty())
            return d->folderProxy->mapToSource(indexes.first());
    }
    return QModelIndex();
}

QModelIndex BookmarksWidget::selectedBookmarkIndex()
{
    QModelIndexList indexes = d->tableView->selectionModel()->selectedIndexes();
    if (!indexes.isEmpty() && d->model) {
        QModelIndex index = d->proxyModel->mapToSource(indexes.first());
        if (!d->model->isFolder(index))
            return index;
    }

    return QModelIndex();
}
