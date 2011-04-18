#include "filemanagerwidget.h"

#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QColumnView>
#include <QtGui/QFileSystemModel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>
#include <QtGui/QTableView>
#include <QtGui/QTreeView>
#include <QtGui/QUndoCommand>
#include <QtGui/QUndoStack>

#include <QDebug>

#include "filesystemundomanager.h"
#include "filesystemmodel.h"

#define MaxViews 5

namespace FileManagerPlugin {

class FileManagerWidgetPrivate
{
    Q_DECLARE_PUBLIC(FileManagerWidget)
    FileManagerWidget *q_ptr;
public:
    FileManagerWidgetPrivate(FileManagerWidget *qq) : q_ptr(qq) {}

    FileManagerWidget::ViewMode viewMode;
    QAbstractItemView * currentView;
    QAbstractItemView * views[MaxViews];
    QStackedLayout * layout;

    QFileSystemModel *model;
    QString currentPath;

    FileSystemUndoManager *undoManager;

    QStringList selectedPaths();
};

} // namespace FileManagerPlugin

using namespace FileManagerPlugin;

QStringList FileManagerWidgetPrivate::selectedPaths()
{
    QStringList result;
    QModelIndexList list = currentView->selectionModel()->selectedRows();

    foreach (QModelIndex index, list) {
        result.append(model->filePath(index));
    }
    return result;
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
    iconView->setMovement(QListView::Static);
    iconView->setResizeMode(QListView::Adjust);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setRootIsDecorated(false);

    d->model = 0;
    d->currentView = 0;
    d->layout = new QStackedLayout(this);

    d->views[ListView] = listView;
    d->views[IconView] = iconView;
    d->views[ColumnView] = columnView;
    d->views[TableView] = tableView;
    d->views[TreeView] = treeView;

    for (int i = 0; i < MaxViews; i++) {
        d->layout->addWidget(d->views[i]);
        d->views[i]->setFocusProxy(this);
        d->views[i]->setSelectionMode(QAbstractItemView::ContiguousSelection);
        d->views[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        d->views[i]->setDragDropMode(QAbstractItemView::DragDrop);
        d->views[i]->setAcceptDrops(true);
//        d->views[i]->setDragEnabled(true);
    }

    d->layout->setContentsMargins(0, 0, 0, 0);
    setLayout(d->layout);

    d->undoManager = new FileSystemUndoManager(this);
    connect(d->undoManager->undoStack(), SIGNAL(canUndoChanged(bool)), SIGNAL(canUndoChanged(bool)));

    FileSystemModel *model = new FileSystemModel(this);
    model->setRootPath("/");
    model->setFilter(QDir::AllEntries | /*QDir::NoDotAndDotDot |*/ QDir::AllDirs | QDir::Hidden);
    model->setReadOnly(false);
    setModel(model);

    setViewMode(ListView);
}

FileManagerWidget::~FileManagerWidget()
{
    delete d_ptr;
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
        d->currentView->setRootIndex(index);
        emit currentPathChanged(path);
    }
}

void FileManagerWidget::copy()
{
    Q_D(FileManagerWidget);

    QClipboard * clipboard = QApplication::clipboard();
    QMimeData * data = new QMimeData();
    QList<QUrl> urls;

    QStringList paths = d->selectedPaths();
    qDebug() << paths;

    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    data->setUrls(urls);
    clipboard->setMimeData(data);
}

void FileManagerWidget::paste()
{
    Q_D(FileManagerWidget);

    // TODO: use QtFileCopier
    QClipboard * clipboard = QApplication::clipboard();
    const QMimeData * data = clipboard->mimeData();
    QList<QUrl> urls = data->urls();
    QDir dir(currentPath());

    CopyCommand * command = new CopyCommand;
    command->setDestination(currentPath());

    foreach (QUrl path, urls) {
        QString filePath = path.toLocalFile();
        command->appendSourcePath(filePath);
    }
    d->undoManager->undoStack()->push(command);
}

void FileManagerWidget::remove()
{
    Q_D(FileManagerWidget);

    QStringList paths = d->selectedPaths();
    foreach (const QString &path, paths) {
        bool result = QFile::remove(path);
        if (!result) {
        }
    }
}

void FileManagerWidget::undo()
{
    Q_D(FileManagerWidget);

    d->undoManager->undoStack()->undo();
}

void FileManagerWidget::redo()
{
    Q_D(FileManagerWidget);

    d->undoManager->undoStack()->redo();
}
