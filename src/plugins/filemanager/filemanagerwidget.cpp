#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtGui/QAction>
#include <QtGui/QHeaderView>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

#include <guisystem/actionmanager.h>
#include <io/QDriveInfo>
#include <widgets/coverflow.h>

#include <core/constants.h>

#include "fileinfodialog.h"
#include "filemanagersettings.h"
#include "filemanagersettings_p.h"

using namespace GuiSystem;
using namespace FileManager;

void FileManagerWidgetPrivate::setupUi()
{
    Q_Q(FileManagerWidget);

    layout = new QStackedLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);

    initViews();

    q->setLayout(layout);
    q->setFocusPolicy(Qt::StrongFocus);
    q->setMinimumSize(200, 200);

    createActions();
    retranslateUi();
}

void FileManagerWidgetPrivate::createActions()
{
    Q_Q(FileManagerWidget);

    actions[FileManagerWidget::Open] = new QAction(this);
    actions[FileManagerWidget::Open]->setEnabled(false);
    connect(actions[FileManagerWidget::Open], SIGNAL(triggered()), q, SLOT(open()));

    actions[FileManagerWidget::OpenInTab] = new QAction(this);
    actions[FileManagerWidget::OpenInTab]->setEnabled(false);
    connect(actions[FileManagerWidget::OpenInTab], SIGNAL(triggered()), this, SLOT(openNewTab()));

    actions[FileManagerWidget::OpenInWindow] = new QAction(this);
    actions[FileManagerWidget::OpenInWindow]->setEnabled(false);
    connect(actions[FileManagerWidget::OpenInWindow], SIGNAL(triggered()), this, SLOT(openNewWindow()));

    actions[FileManagerWidget::SelectProgram] = new QAction(this);
    connect(actions[FileManagerWidget::SelectProgram], SIGNAL(triggered()), q, SLOT(selectProgram()));

    actions[FileManagerWidget::NewFolder] = new QAction(this);
    connect(actions[FileManagerWidget::NewFolder], SIGNAL(triggered()), q, SLOT(newFolder()));

    actions[FileManagerWidget::Rename] = new QAction(this);
    actions[FileManagerWidget::Rename]->setEnabled(false);
    connect(actions[FileManagerWidget::Rename], SIGNAL(triggered()), q, SLOT(rename()));

    actions[FileManagerWidget::Remove] = new QAction(this);
    actions[FileManagerWidget::Remove]->setEnabled(false);
    connect(actions[FileManagerWidget::Remove], SIGNAL(triggered()), q, SLOT(remove()));

    actions[FileManagerWidget::ShowFileInfo] = new QAction(this);
    connect(actions[FileManagerWidget::ShowFileInfo], SIGNAL(triggered()), q, SLOT(showFileInfo()));

    actions[FileManagerWidget::Redo] = new QAction(this);
    actions[FileManagerWidget::Redo]->setEnabled(false);
    connect(actions[FileManagerWidget::Redo], SIGNAL(triggered()), q, SLOT(redo()));
    connect(q, SIGNAL(canRedoChanged(bool)), actions[FileManagerWidget::Redo], SLOT(setEnabled(bool)));

    actions[FileManagerWidget::Undo] = new QAction(this);
    actions[FileManagerWidget::Undo]->setEnabled(false);
    connect(actions[FileManagerWidget::Undo], SIGNAL(triggered()), q, SLOT(undo()));
    connect(q, SIGNAL(canUndoChanged(bool)), actions[FileManagerWidget::Undo], SLOT(setEnabled(bool)));

    actions[FileManagerWidget::Cut] = new QAction(this);
    actions[FileManagerWidget::Cut]->setEnabled(false);
//    connect(actions[FileManagerWidget::Cut], SIGNAL(triggered()), q, SLOT(cut()));

    actions[FileManagerWidget::Copy] = new QAction(this);
    connect(actions[FileManagerWidget::Copy], SIGNAL(triggered()), q, SLOT(copy()));

    actions[FileManagerWidget::Paste] = new QAction(this);
    connect(actions[FileManagerWidget::Paste], SIGNAL(triggered()), q, SLOT(paste()));

    actions[FileManagerWidget::SelectAll] = new QAction(this);
    connect(actions[FileManagerWidget::SelectAll], SIGNAL(triggered()), q, SLOT(selectAll()));

    actions[FileManagerWidget::ShowHiddenFiles] = new QAction(this);
    actions[FileManagerWidget::ShowHiddenFiles]->setCheckable(true);
    connect(actions[FileManagerWidget::ShowHiddenFiles], SIGNAL(triggered(bool)), q, SLOT(showHiddenFiles(bool)));

    viewModeGroup = new QActionGroup(this);

    actions[FileManagerWidget::IconMode] = new QAction(this);
    actions[FileManagerWidget::ColumnMode] = new QAction(this);
    actions[FileManagerWidget::TreeMode] = new QAction(this);
    actions[FileManagerWidget::CoverFlowMode] = new QAction(this);

    actions[FileManagerWidget::IconMode]->setCheckable(true);
    actions[FileManagerWidget::IconMode]->setChecked(true);
    actions[FileManagerWidget::ColumnMode]->setCheckable(true);
    actions[FileManagerWidget::TreeMode]->setCheckable(true);
    actions[FileManagerWidget::CoverFlowMode]->setCheckable(true);

    viewModeGroup->addAction(actions[FileManagerWidget::IconMode]);
    viewModeGroup->addAction(actions[FileManagerWidget::ColumnMode]);
    viewModeGroup->addAction(actions[FileManagerWidget::TreeMode]);
    viewModeGroup->addAction(actions[FileManagerWidget::CoverFlowMode]);

    actions[FileManagerWidget::IconMode]->setData(FileManagerWidget::IconView);
    actions[FileManagerWidget::ColumnMode]->setData(FileManagerWidget::ColumnView);
    actions[FileManagerWidget::TreeMode]->setData(FileManagerWidget::TreeView);
    actions[FileManagerWidget::CoverFlowMode]->setData(FileManagerWidget::CoverFlow);

    connect(actions[FileManagerWidget::IconMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));
    connect(actions[FileManagerWidget::ColumnMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));
    connect(actions[FileManagerWidget::TreeMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));
    connect(actions[FileManagerWidget::CoverFlowMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));

    sortByGroup = new QActionGroup(this);

    actions[FileManagerWidget::SortByName] = new QAction(this);
    actions[FileManagerWidget::SortBySize] = new QAction(this);
    actions[FileManagerWidget::SortByType] = new QAction(this);
    actions[FileManagerWidget::SortByDate] = new QAction(this);
    actions[FileManagerWidget::SortDescendingOrder] = new QAction(this);

    actions[FileManagerWidget::SortByName]->setCheckable(true);
    actions[FileManagerWidget::SortBySize]->setCheckable(true);
    actions[FileManagerWidget::SortByType]->setCheckable(true);
    actions[FileManagerWidget::SortByDate]->setCheckable(true);
    actions[FileManagerWidget::SortDescendingOrder]->setCheckable(true);

    actions[FileManagerWidget::SortByName]->setChecked(true);

    sortByGroup->addAction(actions[FileManagerWidget::SortByName]);
    sortByGroup->addAction(actions[FileManagerWidget::SortBySize]);
    sortByGroup->addAction(actions[FileManagerWidget::SortByType]);
    sortByGroup->addAction(actions[FileManagerWidget::SortByDate]);

    actions[FileManagerWidget::SortByName]->setData(FileManagerWidget::NameColumn);
    actions[FileManagerWidget::SortBySize]->setData(FileManagerWidget::SizeColumn);
    actions[FileManagerWidget::SortByType]->setData(FileManagerWidget::TypeColumn);
    actions[FileManagerWidget::SortByDate]->setData(FileManagerWidget::DateColumn);

    connect(actions[FileManagerWidget::SortByName], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[FileManagerWidget::SortBySize], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[FileManagerWidget::SortByType], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[FileManagerWidget::SortByDate], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[FileManagerWidget::SortDescendingOrder], SIGNAL(triggered(bool)), SLOT(toggleSortOrder(bool)));

    for (int i = 0; i < FileManagerWidget::ActionCount; i++) {
        q->addAction(actions[i]);
    }
}

void FileManagerWidgetPrivate::retranslateUi()
{
    actions[FileManagerWidget::Open]->setText(tr("Open"));
    actions[FileManagerWidget::OpenInTab]->setText(tr("Open in new tab"));
    actions[FileManagerWidget::OpenInWindow]->setText(tr("Open in new window"));

    actions[FileManagerWidget::SelectProgram]->setText(tr("Select program..."));
    actions[FileManagerWidget::NewFolder]->setText(tr("New Folder"));
    actions[FileManagerWidget::Rename]->setText(tr("Rename"));
    actions[FileManagerWidget::Remove]->setText(tr("Remove"));
    actions[FileManagerWidget::ShowFileInfo]->setText(tr("File info"));

    actions[FileManagerWidget::Redo]->setText(tr("Redo"));
    actions[FileManagerWidget::Undo]->setText(tr("Undo"));
    actions[FileManagerWidget::Cut]->setText(tr("Cut"));
    actions[FileManagerWidget::Copy]->setText(tr("Copy"));
    actions[FileManagerWidget::Paste]->setText(tr("Paste"));
    actions[FileManagerWidget::SelectAll]->setText(tr("Select all"));

    actions[FileManagerWidget::ShowHiddenFiles]->setText(tr("Show hidden files"));

    actions[FileManagerWidget::IconMode]->setText(tr("Icon view"));
    actions[FileManagerWidget::ColumnMode]->setText(tr("Column view"));
    actions[FileManagerWidget::TreeMode]->setText(tr("Tree view"));
    actions[FileManagerWidget::CoverFlowMode]->setText(tr("Cover flow"));

    actions[FileManagerWidget::SortByName]->setText(tr("Sort by name"));
    actions[FileManagerWidget::SortBySize]->setText(tr("Sort by size"));
    actions[FileManagerWidget::SortByType]->setText(tr("Sort by type"));
    actions[FileManagerWidget::SortByDate]->setText(tr("Sort by date"));
    actions[FileManagerWidget::SortDescendingOrder]->setText(tr("Descending order"));
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

void FileManagerWidgetPrivate::initViews()
{
    Q_Q(FileManagerWidget);

    QListView *iconView = new ListView(q);
    QColumnView *columnView = new QColumnView(q);
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

    treeView->setAlternatingRowColors(true);
    treeView->setExpandsOnDoubleClick(false);
    treeView->setSortingEnabled(true);
    connect(treeView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));

    coverFlow->setPictureColumn(0);
    coverFlow->setPictureRole(Qt::UserRole);

    views[FileManagerWidget::IconView] = iconView;
    views[FileManagerWidget::ColumnView] = columnView;
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
    connect(q, SIGNAL(selectedPathsChanged()), SLOT(onSelectionChanged()));
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

    model->sort(sortingColumn, sortingOrder);
}

void FileManagerWidgetPrivate::registerAction(QAction *action, const QByteArray &id)
{
    GuiSystem::ActionManager::instance()->registerAction(action, id);
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

#include "filemanagersettings.h"
FileManagerWidget::FileManagerWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileManagerWidgetPrivate(this))
{
    Q_D(FileManagerWidget);

    qRegisterMetaType<ViewMode>("ViewMode");

    d->setupUi();

    d->model = 0;
    d->currentView = 0;
    d->viewMode = (FileManagerWidget::ViewMode)-1; // to skip if in setView()
    d->fileSystemManager = 0;
    d->sortingColumn = (FileManagerWidget::Column)-1;
    d->sortingOrder = (Qt::SortOrder)-1;
    d->itemsExpandable = true;

    d->history = new History(this);
    connect(d->history, SIGNAL(currentItemIndexChanged(int)), d, SLOT(onCurrentItemIndexChanged(int)));

    FileSystemModel *model = new FileSystemModel(this);
    model->setRootPath("/");
    model->setFilter(mBaseFilters);
    model->setReadOnly(false);
    d->setModel(model);
    d->setFileSystemManager(FileSystemManager::instance());

//    ((QTreeView*)d->views[FileManagerWidget::TableView])->setColumnWidth(0, 250);
    ((QTreeView*)d->views[FileManagerWidget::TreeView])->setColumnWidth(0, 250);

    FileManagerSettings *settings = FileManagerSettings::globalSettings();

    setViewMode(IconView);
    setFlow((Flow)settings->flow());
    setIconSize(IconView, settings->iconSize(FileManagerSettings::IconView));
    setIconSize(ColumnView, settings->iconSize(FileManagerSettings::ColumnView));
    setIconSize(TreeView, settings->iconSize(FileManagerSettings::TreeView));
    setGridSize(settings->gridSize());
    setItemsExpandable(settings->itemsExpandable());
    setSorting(NameColumn, Qt::AscendingOrder);

    FileManagerSettings::globalSettings()->d_func()->addWidget(this);
}

FileManagerWidget::~FileManagerWidget()
{
    FileManagerSettings::globalSettings()->d_func()->removeWidget(this);

    delete d_ptr;
}

QAction * FileManagerWidget::action(Action action) const
{
    Q_D(const FileManagerWidget);

    if (action <= NoAction || action >= ActionCount)
        return 0;

    return d->actions[action];
}

bool FileManagerWidget::alternatingRowColors() const
{
    Q_D(const FileManagerWidget);

    return d->views[TreeView]->alternatingRowColors();
}

void FileManagerWidget::setAlternatingRowColors(bool enable)
{
    Q_D(FileManagerWidget);

    d->views[TreeView]->setAlternatingRowColors(enable);
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

QSize FileManagerWidget::iconSize(FileManagerWidget::ViewMode mode) const
{
    Q_D(const FileManagerWidget);

    return d->views[mode]->iconSize();
}

void FileManagerWidget::setIconSize(FileManagerWidget::ViewMode mode, QSize size)
{
    Q_D(FileManagerWidget);

    if (mode < 0 && mode >= FileManagerWidget::MaxViews)
        return;

    d->views[mode]->setIconSize(size);
}

bool FileManagerWidget::itemsExpandable() const
{
    Q_D(const FileManagerWidget);

    return d->itemsExpandable;
}

void FileManagerWidget::setItemsExpandable(bool expandable)
{
    Q_D(FileManagerWidget);

    if (d->itemsExpandable == expandable)
        return;

    d->itemsExpandable = expandable;

    QTreeView *view = static_cast<QTreeView *>(d->views[TreeView]);
    if (!expandable) {
        view->collapseAll();
        view->setRootIsDecorated(false);
        view->setItemsExpandable(false);
    } else {
        view->setRootIsDecorated(true);
        view->setItemsExpandable(true);
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

    for (int i = NameColumn; i < ColumnCount; i++) {
        d->actions[SortByName + i]->setChecked(column == NameColumn + i);
    }

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

    d->actions[SortDescendingOrder]->setChecked(order == Qt::DescendingOrder);

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

void FileManagerWidget::setViewMode(ViewMode mode)
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

        d->actions[IconMode]->setChecked(mode == IconView);
        d->actions[ColumnMode]->setChecked(mode == ColumnView);
        d->actions[TreeMode]->setChecked(mode == TreeView);
        d->actions[CoverFlowMode]->setChecked(mode == CoverFlow);

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

bool FileManagerWidget::restoreState(const QByteArray &arr)
{
    if (arr.isEmpty())
        return false;

    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    quint8 tmp;
    QSize size;
    s >> tmp;
    setFlow((Flow)tmp);
    s >> size;
    setGridSize(size);
    s >> size;
    setIconSize(IconView, size);
    s >> tmp;
    setViewMode((FileManagerWidget::ViewMode)tmp);
    s >> tmp;
    setSortingColumn((FileManagerWidget::Column)tmp);
    s >> tmp;
    setSortingOrder((Qt::SortOrder)tmp);
    return true;
}

QByteArray FileManagerWidget::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << (quint8)flow();
    s << gridSize();
    s << iconSize(IconView);
    s << (quint8)viewMode();
    s << (quint8)sortingColumn();
    s << (quint8)sortingOrder();

    return state;
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

void FileManagerWidget::selectProgram()
{
//    emit selectProgramRequested();
    QSettings settings;
    QString programsFolder;
    QVariant value = settings.value(QLatin1String("filemanager/programsFolder"));
    if (value.isValid()) {
        programsFolder = value.toString();
    } else {
#if defined(Q_OS_MAC)
        programsFolder = QLatin1String("/Applications");
#elif defined(Q_OS_WIN)
        programsFolder = QDriveInfo::rootDrive().rootPath() + QLatin1String("/Program Files");
#elif defined(Q_OS_UNIX)
        programsFolder = QLatin1String("/usr/bin");
#endif
    }

#ifdef Q_OS_WIN
    QString filter = tr("Programs (*.exe *.cmd *.com *.bat);;All files (*)");
#else
    QString filter;
#endif

    QString programPath = QFileDialog::getOpenFileName(this, tr("Select program"), programsFolder, filter);
    if (programPath.isEmpty())
        return;

    settings.setValue(QLatin1String("filemanager/programsFolder"), QFileInfo(programPath).absolutePath());

    bool result = true;
    QStringList failedPaths;
    foreach (const QString path, selectedPaths()) {
        QString program;
        QStringList arguments;
#if defined(Q_OS_MAC)
        program = QLatin1String("open");
        arguments << QLatin1String("-a") << programPath << path;
#else
        program = programPath;
        arguments << path;
#endif
        bool r = QProcess::startDetached(program, arguments);
        if (!r)
            failedPaths.append(path);
        result &= r;
    }

    if (!result) {
        QMessageBox::warning(this,
                             tr("Can't open files"),
                             tr("Andromeda failed to open some files :%1").
                             arg(failedPaths.join(QLatin1String("\n"))));
    }
}

void FileManagerWidget::showFileInfo()
{
    QStringList paths = selectedPaths();
    if (paths.isEmpty())
        paths.append(currentPath());

    foreach (const QString &path, paths) {
        FileInfoDialog *dialog = new FileInfoDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setFileInfo(QFileInfo(path));
        dialog->show();
    }
}

void FileManagerWidgetPrivate::openNewTab()
{
    Q_Q(FileManagerWidget);

    QStringList paths = q->selectedPaths();

    if (!paths.isEmpty())
        emit q->openNewTabRequested(paths);
}

void FileManagerWidgetPrivate::openNewWindow()
{
    Q_Q(FileManagerWidget);

    QStringList paths = q->selectedPaths();

    if (!paths.isEmpty())
        emit q->openNewWindowRequested(paths);
}

void FileManagerWidgetPrivate::toggleViewMode(bool toggled)
{
    Q_Q(FileManagerWidget);

    if (toggled) {
        QAction *action = qobject_cast<QAction*>(sender());
        if (action)
            q->setViewMode((FileManagerWidget::ViewMode)action->data().toInt());
    }
}

void FileManagerWidgetPrivate::toggleSortColumn(bool toggled)
{
    Q_Q(FileManagerWidget);

    if (toggled) {
        QAction *action = qobject_cast<QAction*>(sender());
        if (action)
            q->setSortingColumn((FileManagerWidget::Column)action->data().toInt());
    }
}

void FileManagerWidgetPrivate::toggleSortOrder(bool descending)
{
    Q_Q(FileManagerWidget);

    q->setSortingOrder(descending ? Qt::DescendingOrder : Qt::AscendingOrder);
}

/*!
  \internal

  Updates FileManager actions that depends on selection.
*/
void FileManagerWidgetPrivate::onSelectionChanged()
{
    Q_Q(FileManagerWidget);

    QStringList paths = q->selectedPaths();
    bool enabled = !paths.empty();

    actions[FileManagerWidget::Open]->setEnabled(enabled);
    actions[FileManagerWidget::OpenInTab]->setEnabled(enabled);
    actions[FileManagerWidget::OpenInWindow]->setEnabled(enabled);
    actions[FileManagerWidget::Rename]->setEnabled(enabled);
    actions[FileManagerWidget::Remove]->setEnabled(enabled);
//    actions[FileManagerWidget::Cut]->setEnabled(enabled);
    actions[FileManagerWidget::Copy]->setEnabled(enabled);

    if (!paths.isEmpty()) {
        if (paths.size() == 1) {
            actions[FileManagerWidget::Cut]->setText(tr("Cut \"%1\"").arg(QFileInfo(paths[0]).fileName()));
            actions[FileManagerWidget::Copy]->setText(tr("Copy \"%1\"").arg(QFileInfo(paths[0]).fileName()));
        } else {
            actions[FileManagerWidget::Cut]->setText(tr("Cut %1 items").arg(paths.size()));
            actions[FileManagerWidget::Copy]->setText(tr("Copy %1 items").arg(paths.size()));
        }
    } else {
        actions[FileManagerWidget::Cut]->setText(tr("Cut"));
        actions[FileManagerWidget::Copy]->setText(tr("Copy"));
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

/*!
  \brief Shows FileManagerWidget's context menu.
*/
void FileManagerWidget::showContextMenu(QPoint pos)
{
    Q_D(FileManagerWidget);
    QStringList paths = selectedPaths();

    QMenu *menu = new QMenu;
    if (paths.isEmpty()) {
        menu->addAction(d->actions[NewFolder]);
        menu->addSeparator();
        menu->addAction(d->actions[ShowFileInfo]);
        menu->addSeparator();
        menu->addAction(d->actions[Paste]);
        menu->addAction(d->actions[SelectAll]);
        menu->addSeparator();
        QMenu * viewModeMenu = menu->addMenu(tr("View Mode"));
        viewModeMenu->addAction(d->actions[IconMode]);
        viewModeMenu->addAction(d->actions[ColumnMode]);
        viewModeMenu->addAction(d->actions[TreeMode]);
        viewModeMenu->addAction(d->actions[CoverFlowMode]);
        QMenu * sortByMenu = menu->addMenu(tr("Sort by"));
        sortByMenu->addAction(d->actions[SortByName]);
        sortByMenu->addAction(d->actions[SortByType]);
        sortByMenu->addAction(d->actions[SortBySize]);
        sortByMenu->addAction(d->actions[SortByDate]);
        sortByMenu->addSeparator();
        sortByMenu->addAction(d->actions[SortDescendingOrder]);
    } else {
        menu->addAction(d->actions[Open]);
        menu->addAction(d->actions[OpenInTab]);
        menu->addAction(d->actions[OpenInWindow]);

        QMenu *openWithMenu = menu->addMenu(tr("Open with"));
        openWithMenu->addSeparator();
        openWithMenu->addAction(d->actions[SelectProgram]);

        menu->addSeparator();
        menu->addAction(d->actions[ShowFileInfo]);
        menu->addSeparator();
        menu->addAction(d->actions[Rename]);
        menu->addAction(d->actions[Remove]);
        menu->addSeparator();
        menu->addAction(d->actions[Copy]);
    }
    menu->exec(mapToGlobal(pos));
    delete menu;
}

void FileManagerWidget::contextMenuEvent(QContextMenuEvent *e)
{
    showContextMenu(e->pos());
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
