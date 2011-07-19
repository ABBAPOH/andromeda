#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"

#include <QtGui/QKeyEvent>
#include <QDebug>

using namespace FileManagerPlugin;

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

FileManagerWidget::FileManagerWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileManagerWidgetPrivate(this))
{
    Q_D(FileManagerWidget);

    QListView *listView = new QListView(this);
    QListView *iconView = new QListView(this);
    QColumnView *columnView = new QColumnView(this);
//    QTableView *tableView = new QTableView(this);
    QTreeView *tableView = new QTreeView(this);
    QTreeView *treeView = new QTreeView(this);

//    iconView->setGridSize(QSize(150, 100));
    iconView->setGridSize(QSize(100, 100));
    iconView->setWordWrap(true);
    iconView->setViewMode(QListView::IconMode);
    iconView->setFlow(QListView::LeftToRight);
    iconView->setMovement(QListView::Snap);
//    iconView->setMovement(QListView::Static);
    iconView->setResizeMode(QListView::Adjust);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setRootIsDecorated(false);
    tableView->setItemsExpandable(false);

    d->model = 0;
    d->currentView = 0;
    d->viewMode = (FileManagerWidget::ViewMode)-1; // to skip if in setView()
    d->layout = new QStackedLayout(this);

    d->views[ListView] = listView;
    d->views[IconView] = iconView;
    d->views[ColumnView] = columnView;
    d->views[TableView] = tableView;
    d->views[TreeView] = treeView;
    d->blockEvents = false;

    setFocusPolicy(Qt::StrongFocus);
    for (int i = 0; i < MaxViews; i++) {
        d->views[i]->setFocusProxy(this);
        d->views[i]->setSelectionMode(QAbstractItemView::ContiguousSelection);
        d->views[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        d->views[i]->setDragDropMode(QAbstractItemView::DragDrop);
        d->views[i]->setAcceptDrops(true);
        d->views[i]->setEditTriggers(QAbstractItemView::SelectedClicked);
        d->layout->addWidget(d->views[i]);
//        d->views[i]->setDragEnabled(true);
        connect(d->views[i], SIGNAL(doubleClicked(QModelIndex)),
                d, SLOT(onDoubleClick(QModelIndex)),
                Qt::QueuedConnection);
    }

    d->layout->setContentsMargins(0, 0, 0, 0);
    setLayout(d->layout);

    d->fileSystemManager = FileSystemManager::instance();
    connect(d->fileSystemManager->undoStack(), SIGNAL(canUndoChanged(bool)),
            SIGNAL(canUndoChanged(bool)));
    connect(d->fileSystemManager->undoStack(), SIGNAL(canRedoChanged(bool)),
            SIGNAL(canRedoChanged(bool)));

    d->history = new History(this);
    connect(d->history, SIGNAL(currentItemIndexChanged(int)), d, SLOT(onCurrentItemIndexChanged(int)));

    FileSystemModel *model = new FileSystemModel(this);
    model->setRootPath("/");
    model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Hidden);
    model->setReadOnly(false);
    setModel(model);

    setViewMode(ListView);

    setMinimumSize(200, 200);
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
            d->currentView->setRootIndex(index);

            HistoryItem item(path, QFileInfo(path).fileName(), QIcon(), QDateTime::currentDateTime());
            d->history->appendItem(item);

            emit currentPathChanged(path);
        }
    }
}

FileSystemManager * FileManagerWidget::fileSystemManager() const
{
    return d_func()->fileSystemManager;
}

History * FileManagerWidget::history() const
{
    Q_D(const FileManagerWidget);

    return d->history;
}

QFileSystemModel * FileManagerWidget::model() const
{
    Q_D(const FileManagerWidget);

    return d->model;
}

void FileManagerWidget::setModel(QFileSystemModel *model)
{
    Q_D(FileManagerWidget);

    d->model = model;

    for (int i = 0; i < MaxViews; i++) {
        d->views[i]->setModel(model);
    }
}

QStringList FileManagerWidget::selectedPaths() const
{
    Q_D(const FileManagerWidget);

    QStringList result;
    QModelIndexList list = d->currentView->selectionModel()->selectedRows();

    foreach (QModelIndex index, list) {
        result.append(d->model->filePath(index));
    }
    return result;
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
        d->layout->setCurrentIndex(mode);
        d->currentView = d->views[mode];

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

void FileManagerWidget::undo()
{
    fileSystemManager()->undoStack()->undo();
}

void FileManagerWidget::redo()
{
    fileSystemManager()->undoStack()->redo();
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
    d->fileSystemManager->copyFiles(files, currentPath());
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

