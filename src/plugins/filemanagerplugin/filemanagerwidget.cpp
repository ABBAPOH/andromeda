#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QAction>
//#include <QDebug>

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

void FileManagerWidgetPrivate::initViews()
{
    Q_Q(FileManagerWidget);

    QListView *listView = new QListView(q);
    QListView *iconView = new QListView(q);
    QColumnView *columnView = new QColumnView(q);
//    QTableView *tableView = new QTableView(q);
    QTreeView *tableView = new QTreeView(q);
    QTreeView *treeView = new QTreeView(q);

    iconView->setLayoutDirection(Qt::LeftToRight);
    iconView->setViewMode(QListView::IconMode);
    iconView->setGridSize(QSize(100, 100));
    iconView->setWordWrap(true);
    iconView->setFlow(QListView::LeftToRight);

    iconView->setResizeMode(QListView::Adjust);
    iconView->setMovement(QListView::Static);
    iconView->setDragEnabled(true);
    iconView->viewport()->setAcceptDrops(true);
    iconView->setMouseTracking(true);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setRootIsDecorated(false);
    tableView->setItemsExpandable(false);

    views[FileManagerWidget::ListView] = listView;
    views[FileManagerWidget::IconView] = iconView;
    views[FileManagerWidget::ColumnView] = columnView;
    views[FileManagerWidget::TableView] = tableView;
    views[FileManagerWidget::TreeView] = treeView;
    blockEvents = false;

    for (int i = 0; i < MaxViews; i++) {
        views[i]->setFocusProxy(q);
        views[i]->setSelectionMode(QAbstractItemView::ContiguousSelection);
        views[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        views[i]->setDragDropMode(QAbstractItemView::DragDrop);
        views[i]->setAcceptDrops(true);
        views[i]->setEditTriggers(QAbstractItemView::SelectedClicked);
//        QAbstractItemView::InternalMove
        layout->addWidget(views[i]);
//        views[i]->setDragEnabled(true);
        connect(views[i], SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(onDoubleClick(QModelIndex)),
                Qt::QueuedConnection);
    }
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

QModelIndexList FileManagerWidgetPrivate::selectedIndexes() const
{
    return currentView->selectionModel()->selectedRows();
}

void FileManagerWidgetPrivate::onDoubleClick(const QModelIndex &index)
{
    Q_Q(FileManagerWidget);

    QString path = model->filePath(index);
    QFileInfo info(path);
    if (info.isDir()) {
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

static QDir::Filters mBaseFilters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs;

// todo : uncomment when c++0x will be standard
//FileManagerWidget::FileManagerWidget(QWidget *parent) :
//    FileManagerWidget(0, parent)
//{
//}

FileManagerWidget::FileManagerWidget(FileSystemModel *model, QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileManagerWidgetPrivate(this))
{
    Q_D(FileManagerWidget);

    d->setupUi();

    d->model = 0;
    d->currentView = 0;
    d->viewMode = (FileManagerWidget::ViewMode)-1; // to skip if in setView()
    d->fileSystemManager = 0;

    d->history = new CorePlugin::History(this);
    connect(d->history, SIGNAL(currentItemIndexChanged(int)), d, SLOT(onCurrentItemIndexChanged(int)));

    if (!model)
        model = new FileSystemModel(this);

    model->setRootPath("/");
    model->setFilter(mBaseFilters);
    model->setReadOnly(false);
    setModel(model);

    setViewMode(ListView);
}

FileManagerWidget::~FileManagerWidget()
{
    delete d_ptr;
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

            CorePlugin::HistoryItem item;
            item.setPath(path);
            item.setTitle(QFileInfo(path).fileName());
            item.setLastVisited(QDateTime::currentDateTime());
            d->history->appendItem(item);

            emit currentPathChanged(path);
        }
    }
}

FileSystemManager * FileManagerWidget::fileSystemManager() const
{
    return d_func()->fileSystemManager;
}

CorePlugin::History * FileManagerWidget::history() const
{
    Q_D(const FileManagerWidget);

    return d->history;
}

FileSystemModel * FileManagerWidget::model() const
{
    Q_D(const FileManagerWidget);

    return d->model;
}

void FileManagerWidget::setModel(FileSystemModel *model)
{
    Q_D(FileManagerWidget);

    if (model == 0)
        setModel(new FileSystemModel(this));

    if (d->model == model)
        return;

    if (d->model && d->model->QObject::parent() == this)
        delete d->model;

    d->model = model;
    d->setFileSystemManager(model->fileSystemManager());

    for (int i = 0; i < MaxViews; i++) {
        d->views[i]->setModel(model);
    }
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
    }
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
        if (QFileInfo(path).isDir()) {
            setCurrentPath(path);
            return;
        } else {
            emit openRequested(path);
        }
    }
}

bool removePath(const QString &path);

void FileManagerWidget::remove()
{
    Q_D(FileManagerWidget);

    QModelIndexList list = d->currentView->selectionModel()->selectedRows();

    foreach (const QModelIndex &index, list) {
        bool result = d->model->remove(index);
        if (!result) {
        }
    }
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
    Q_D(FileManagerWidget);

    QClipboard * clipboard = QApplication::clipboard();
    const QMimeData * data = clipboard->mimeData();
    const QList<QUrl> & urls = data->urls();

    QStringList files;
    foreach (const QUrl &url, urls) {
        files.append(url.toLocalFile());
    }
    d->fileSystemManager->copy(files, currentPath());
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
