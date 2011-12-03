#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"

#include <QtCore/QBuffer>
#include <QtGui/QKeyEvent>
#include <QtGui/QAction>
#include <widgets/coverflow.h>

using namespace GuiSystem;
using namespace FileManagerPlugin;

void FileManagerWidgetPrivate::setupUi()
{
    Q_Q(FileManagerWidget);

    layout = new QStackedLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);

    initViews();

    q->setLayout(layout);
    q->setFocusPolicy(Qt::StrongFocus);
    q->setMinimumSize(200, 200);
}

class ListView : public QListView
{
public:
    ListView(QWidget *parent = 0) : QListView(parent) {}

    void dropEvent(QDropEvent *e)
    {
        QListView::dropEvent(e);
        doItemsLayout(); // fix for wrong layout after dropping
    }
};

#include <QHeaderView>
void FileManagerWidgetPrivate::initViews()
{
    Q_Q(FileManagerWidget);

    QListView *iconView = new ListView(q);
    QColumnView *columnView = new QColumnView(q);
//    QTableView *tableView = new QTableView(q);
    QTreeView *tableView = new QTreeView(q);
    QTreeView *treeView = new QTreeView(q);
    CoverFlow *coverFlow = new CoverFlow(q);

    iconView->setWordWrap(true);
    iconView->setWrapping(true);
    iconView->setFlow(QListView::LeftToRight);
    iconView->setViewMode(QListView::IconMode);
#ifdef Q_OS_MAC
    iconView->setIconSize(QSize(64, 64));
    iconView->setGridSize(QSize(128, 128));
#else
    iconView->setIconSize(QSize(32, 32));
    iconView->setGridSize(QSize(100, 100));
#endif

    iconView->setResizeMode(QListView::Adjust);
    iconView->setMovement(QListView::Static);
    iconView->setDragEnabled(true);
    iconView->viewport()->setAcceptDrops(true);
    iconView->setMouseTracking(true);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setRootIsDecorated(false);
    tableView->setItemsExpandable(false);
    connect(tableView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));

    treeView->setAlternatingRowColors(true);
    treeView->setExpandsOnDoubleClick(false);
    treeView->setSortingEnabled(true);
    connect(treeView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));

    coverFlow->setPictureColumn(0);
    coverFlow->setPictureRole(Qt::UserRole);

    views[FileManagerWidget::IconView] = iconView;
    views[FileManagerWidget::ColumnView] = columnView;
    views[FileManagerWidget::TableView] = tableView;
    views[FileManagerWidget::TreeView] = treeView;
    views[FileManagerWidget::CoverFlow] = coverFlow->treeView();
    blockEvents = false;

    for (int i = 0; i < FileManagerWidget::MaxViews; i++) {
        views[i]->setFocusProxy(q);
        views[i]->setSelectionMode(QAbstractItemView::ExtendedSelection);
        views[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        views[i]->setDragDropMode(QAbstractItemView::DragDrop);
        views[i]->setAcceptDrops(true);
        views[i]->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
        views[i]->setTextElideMode(Qt::ElideMiddle);
//        QAbstractItemView::InternalMove
//        layout->addWidget(views[i]);
//        views[i]->setDragEnabled(true);
        connect(views[i], SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(onDoubleClick(QModelIndex)),
                Qt::QueuedConnection);
    }

    layout->addWidget(iconView);
    layout->addWidget(columnView);
    layout->addWidget(tableView);
    layout->addWidget(treeView);
    layout->addWidget(coverFlow);
}

void FileManagerWidgetPrivate::setFileSystemManager(FileSystemManager *manager)
{
    Q_Q(FileManagerWidget);

    if (fileSystemManager) {
        disconnect(fileSystemManager, 0, q, 0);
    }

    fileSystemManager = manager;
    connect(fileSystemManager, SIGNAL(canUndoChanged(bool)),
            q, SIGNAL(canUndoChanged(bool)));
    connect(fileSystemManager, SIGNAL(canRedoChanged(bool)),
            q, SIGNAL(canRedoChanged(bool)));
}

void FileManagerWidgetPrivate::setModel(FileSystemModel *m)
{
    Q_Q(FileManagerWidget);

    if (m == 0)
        setModel(new FileSystemModel(this));

    if (model == m)
        return;

    if (model) {
        for (int i = 0; i < FileManagerWidget::MaxViews; i++) {
            disconnect(views[i]->selectionModel(), 0, q, 0);
        }
    }

    if (model && model->QObject::parent() == this)
        delete model;

    model = m;

    for (int i = 0; i < FileManagerWidget::MaxViews; i++) {
        views[i]->setModel(m);
        connect(views[i]->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                q, SIGNAL(selectedPathsChanged()));
    }
}

QModelIndexList FileManagerWidgetPrivate::selectedIndexes() const
{
    if (viewMode == FileManagerWidget::ColumnView)
        return currentView->selectionModel()->selectedIndexes();

    return currentView->selectionModel()->selectedRows();
}

void FileManagerWidgetPrivate::updateSorting()
{
    QTreeView *view = 0;

    view = static_cast<QTreeView*>(views[FileManagerWidget::TreeView]);
    view->sortByColumn(sortingColumn, sortingOrder);

    view = static_cast<QTreeView*>(views[FileManagerWidget::TableView]);
    view->sortByColumn(sortingColumn, sortingOrder);

    model->sort(sortingColumn, sortingOrder);
}

void FileManagerWidgetPrivate::onDoubleClick(const QModelIndex &index)
{
    Q_Q(FileManagerWidget);

    QString path = model->filePath(index);
    QFileInfo info(path);
    if (info.isDir() && !info.isBundle()) {
        q->setCurrentPath(info.absoluteFilePath());
    }  else {
        emit q->openRequested(path);
    }
}

// Triggers when current item in history changes
void FileManagerWidgetPrivate::onCurrentItemIndexChanged(int index)
{
    Q_Q(FileManagerWidget);

    QString path = history->itemAt(index).path();
    if (currentPath != path) {
        currentPath = path;
        QModelIndex modelIndex = model->index(path);
        currentView->setRootIndex(modelIndex);

        emit q->currentPathChanged(path);
    }
}

void FileManagerWidgetPrivate::onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    Q_Q(FileManagerWidget);

    if (sortingColumn == logicalIndex && sortingOrder == order)
        return;

    sortingColumn = (FileManagerWidget::Column)logicalIndex;
    sortingOrder = order;

    emit q->sortingChanged();
}

static QDir::Filters mBaseFilters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs;

FileManagerWidget::FileManagerWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileManagerWidgetPrivate(this))
{
    Q_D(FileManagerWidget);

    d->setupUi();

    d->model = 0;
    d->currentView = 0;
    d->viewMode = (FileManagerWidget::ViewMode)-1; // to skip if in setView()
    d->fileSystemManager = 0;
    d->sortingColumn = (FileManagerWidget::Column)-1;
    d->sortingOrder = (Qt::SortOrder)-1;

    d->history = new History(this);
    connect(d->history, SIGNAL(currentItemIndexChanged(int)), d, SLOT(onCurrentItemIndexChanged(int)));

    FileSystemModel *model = new FileSystemModel(this);
    model->setRootPath("/");
    model->setFilter(mBaseFilters);
    model->setReadOnly(false);
    d->setModel(model);

//    ((QTreeView*)d->views[FileManagerWidget::TableView])->setColumnWidth(0, 250);
    ((QTreeView*)d->views[FileManagerWidget::TreeView])->setColumnWidth(0, 250);

    setViewMode(IconView);
    setFlow(LeftToRight);
    setGridSize(QSize(128, 128));
    setSorting(NameColumn, Qt::AscendingOrder);
}

FileManagerWidget::~FileManagerWidget()
{
    delete d_ptr;
}

bool FileManagerWidget::canRedo() const
{
    return fileSystemManager()->canRedo();
}

bool FileManagerWidget::canUndo() const
{
    return fileSystemManager()->canUndo();
}

QString FileManagerWidget::currentPath() const
{
    Q_D(const FileManagerWidget);

    return d->currentPath;
}

void FileManagerWidget::setCurrentPath(const QString &path)
{
    Q_D(FileManagerWidget);

    if (d->currentPath != path) {
        d->currentPath = path;
        QModelIndex index = d->model->index(path);
        if (d->model->isDir(index)) {
            d->currentView->selectionModel()->clear(); // to prevent bug with selecting dir we enter in
            d->currentView->setRootIndex(index);

            HistoryItem item;
            item.setPath(path);
            item.setTitle(QFileInfo(path).fileName());
            item.setLastVisited(QDateTime::currentDateTime());
            d->history->appendItem(item);

            emit currentPathChanged(path);
        }
    }
}

FileManagerWidget::Flow FileManagerWidget::flow() const
{
    Q_D(const FileManagerWidget);

    return d->flow;
}

void FileManagerWidget::setFlow(FileManagerWidget::Flow flow)
{
    Q_D(FileManagerWidget);

    if (d->flow == flow)
        return;

    QListView *view = (QListView *)d->views[IconView];
    QSize s = d->gridSize;
    if (flow == LeftToRight) {
        view->setFlow(QListView::LeftToRight);
        view->setViewMode(QListView::IconMode);
        view->update();
        d->flow = flow;
    } else if (flow == TopToBottom) {
        view->setFlow(QListView::TopToBottom);
        view->setViewMode(QListView::ListMode);
        d->flow = flow;
        s.setWidth(256);
    }
    view->setGridSize(s);
    // fix dragging
    view->setDragEnabled(true);
    view->viewport()->setAcceptDrops(true);
    view->setMouseTracking(true);
}

QSize FileManagerWidget::gridSize() const
{
    Q_D(const FileManagerWidget);

    return d->gridSize;
}

void FileManagerWidget::setGridSize(QSize s)
{
    Q_D(FileManagerWidget);

    if (d->gridSize == s)
        return;

    d->gridSize = s;
    QListView *view = (QListView *)d->views[IconView];
    if (d->flow == TopToBottom)
        s.setWidth(256);
    view->setGridSize(s);
}

QSize FileManagerWidget::iconSize() const
{
    Q_D(const FileManagerWidget);

    return d->views[IconView]->iconSize();
}

void FileManagerWidget::setIconSize(QSize s)
{
    Q_D(FileManagerWidget);

    d->views[IconView]->setIconSize(s);
}

QStringList FileManagerWidget::selectedPaths() const
{
    Q_D(const FileManagerWidget);

    QStringList result;
    foreach (const QModelIndex &index, d->selectedIndexes()) {
        result.append(d->model->filePath(index));
    }
    return QStringList(result);
}

FileManagerWidget::Column FileManagerWidget::sortingColumn() const
{
    return d_func()->sortingColumn;
}

void FileManagerWidget::setSortingColumn(FileManagerWidget::Column column)
{
    Q_D(FileManagerWidget);

    if (d->sortingColumn == column)
        return;

    d->sortingColumn = column;
    d->updateSorting();
    emit sortingChanged();
}

Qt::SortOrder FileManagerWidget::sortingOrder() const
{
    return d_func()->sortingOrder;
}

void FileManagerWidget::setSortingOrder(Qt::SortOrder order)
{
    Q_D(FileManagerWidget);

    if (d->sortingOrder == order)
        return;

    d->sortingOrder = order;
    d->updateSorting();
    emit sortingChanged();
}

void FileManagerWidget::setSorting(FileManagerWidget::Column column, Qt::SortOrder order)
{
    Q_D(FileManagerWidget);

    if (d->sortingColumn == column && d->sortingOrder == order)
        return;

    d->sortingColumn = column;
    d->sortingOrder = order;
    d->updateSorting();
    emit sortingChanged();
}

FileManagerWidget::ViewMode FileManagerWidget::viewMode() const
{
    Q_D(const FileManagerWidget);

    return d->viewMode;
}

void FileManagerWidget::setViewMode(FileManagerWidget::ViewMode mode)
{
    Q_D(FileManagerWidget);

    if (d->viewMode != mode) {
        d->viewMode = mode;
        bool focus = d->currentView ? d->currentView->hasFocus() : false;
        d->layout->setCurrentIndex(mode);
        d->currentView = d->views[mode];
        if (focus) {
            d->currentView->setFocus();
        }

        QModelIndex index = d->model->index(d->currentPath);
        d->currentView->setRootIndex(index);

        emit viewModeChanged(d->viewMode);
    }
}

FileSystemManager * FileManagerWidget::fileSystemManager() const
{
    Q_D(const FileManagerWidget);

    if (!d->fileSystemManager) {
        const_cast<FileManagerWidgetPrivate*>(d)->setFileSystemManager(d->model->fileSystemManager());
    }

    return d->fileSystemManager;
}

void FileManagerWidget::setFileSystemManager(FileSystemManager *manager)
{
    Q_D(FileManagerWidget);

    if (!d->fileSystemManager) {
        d->model->setFileSystemManager(manager);
        d->setFileSystemManager(manager);
    }
}

History * FileManagerWidget::history() const
{
    Q_D(const FileManagerWidget);

    return d->history;
}

FileSystemModel * FileManagerWidget::model() const
{
    Q_D(const FileManagerWidget);

    return d->model;
}

bool FileManagerWidget::restoreState(const QByteArray &state)
{
    if (state.isEmpty())
        return false;

    QByteArray data = state;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QDataStream s(&buffer);
    quint8 tmp;
    QSize size;
    s >> tmp;
    setFlow((Flow)tmp);
    s >> size;
    setGridSize(size);
    s >> size;
    setIconSize(size);
    s >> tmp;
    setViewMode((FileManagerWidget::ViewMode)tmp);
    s >> tmp;
    setSortingColumn((FileManagerWidget::Column)tmp);
    s >> tmp;
    setSortingOrder((Qt::SortOrder)tmp);
    return true;
}

QByteArray FileManagerWidget::saveState()
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    QDataStream s(&buffer);
    s << (quint8)flow();
    s << gridSize();
    s << iconSize();
    s << (quint8)viewMode();
    s << (quint8)sortingColumn();
    s << (quint8)sortingOrder();

    return buffer.data();
}

void FileManagerWidget::newFolder()
{
    Q_D(FileManagerWidget);
    QString dir = currentPath();
    if (dir == "") {
//        emit error;
    } else {
        QString  folderName = tr("New Folder");
        QModelIndex index = d->model->mkdir(d->model->index(dir), folderName);

        if (index.isValid())
            d->currentView->edit(index);
    }
}

void FileManagerWidget::open()
{
    foreach (const QString path, selectedPaths()) {
        QFileInfo info(path);
        if (info.isDir() && !info.isBundle()) {
            setCurrentPath(path);
            return;
        } else {
            emit openRequested(path);
        }
    }
}

void FileManagerWidget::remove()
{
    fileSystemManager()->remove(selectedPaths());
}

void FileManagerWidget::rename()
{
    Q_D(FileManagerWidget);

    QModelIndexList indexes = d->selectedIndexes();
    if (indexes.count() != 1) {
//        emit error();
    } else {
        d->currentView->edit(indexes.first());
    }
}

void FileManagerWidget::undo()
{
    fileSystemManager()->undo();
}

void FileManagerWidget::redo()
{
    fileSystemManager()->redo();
}

void FileManagerWidget::copy()
{
    QClipboard * clipboard = QApplication::clipboard();
    QMimeData * data = new QMimeData();
    QList<QUrl> urls;

    QStringList paths = selectedPaths();

    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    data->setUrls(urls);
    clipboard->setMimeData(data);
}

void FileManagerWidget::paste()
{
    QClipboard * clipboard = QApplication::clipboard();
    const QMimeData * data = clipboard->mimeData();
    const QList<QUrl> & urls = data->urls();

    QStringList files;
    foreach (const QUrl &url, urls) {
        files.append(url.toLocalFile());
    }
    fileSystemManager()->copy(files, currentPath());
}

void FileManagerWidget::selectAll()
{
    Q_D(FileManagerWidget);

    d->currentView->selectAll();
}

void FileManagerWidget::back()
{
    history()->back();
}

void FileManagerWidget::forward()
{
    history()->forward();
}

void FileManagerWidget::up()
{
    Q_D(FileManagerWidget);

    QDir dir(d->currentPath);
    dir.cdUp();
    setCurrentPath(dir.path());
}

void FileManagerWidget::showHiddenFiles(bool show)
{
    Q_D(FileManagerWidget);

    if (show)
        d->model->setFilter(mBaseFilters | QDir::Hidden);
    else
        d->model->setFilter(mBaseFilters);
}

void FileManagerWidget::keyPressEvent(QKeyEvent *event)
{
    Q_D(FileManagerWidget);

#ifndef Q_OS_MAC
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        open();
#endif

    if (!d->blockEvents) { // prevent endless recursion
        d->blockEvents = true;
        qApp->sendEvent(d_func()->currentView, event);
        d->blockEvents = false;
    } else {
        d->blockEvents = false;
    }
}

void FileManagerWidget::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(FileManagerWidget);

    if (!d->blockEvents) { // prevent endless recursion
        d->blockEvents = true;
        qApp->sendEvent(d_func()->currentView, event);
        d->blockEvents = false;
    } else {
        d->blockEvents = false;
    }
}
