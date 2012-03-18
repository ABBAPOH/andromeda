#include "dualpanewidget.h"
#include "dualpanewidget_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QEvent>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "filemanagerwidget.h"
#include "filesystemmanager.h"

using namespace GuiSystem;
using namespace FileManager;

void swapPalettes(QWidget *active, QWidget *inactive)
{
    QPalette palette = inactive->palette(); // it is active palette:)
    active->setPalette(palette);
    QColor color = palette.color(QPalette::Disabled, QPalette::Base);
    palette.setColor(QPalette::Active, QPalette::Base, color);
    inactive->setPalette(palette);
}

void DualPaneWidgetPrivate::createActions()
{
    Q_Q(DualPaneWidget);

    actions[DualPaneWidget::Open] = new QAction(this);
    actions[DualPaneWidget::Open]->setEnabled(false);
    connect(actions[DualPaneWidget::Open], SIGNAL(triggered()), q, SLOT(open()));

    actions[DualPaneWidget::OpenInTab] = new QAction(this);
    connect(actions[DualPaneWidget::OpenInTab], SIGNAL(triggered()), this, SLOT(openNewTab()));

    actions[DualPaneWidget::OpenInWindow] = new QAction(this);
    connect(actions[DualPaneWidget::OpenInWindow], SIGNAL(triggered()), this, SLOT(openNewWindow()));

    actions[DualPaneWidget::SelectProgram] = new QAction(this);
    connect(actions[DualPaneWidget::SelectProgram], SIGNAL(triggered()), q, SLOT(selectProgram()));

    actions[DualPaneWidget::NewFolder] = new QAction(this);
    connect(actions[DualPaneWidget::NewFolder], SIGNAL(triggered()), q, SLOT(newFolder()));

    actions[DualPaneWidget::CopyFiles] = new QAction(this);
    actions[DualPaneWidget::CopyFiles]->setShortcut(QKeySequence("F5"));
    actions[DualPaneWidget::CopyFiles]->setEnabled(false);
    connect(actions[DualPaneWidget::CopyFiles], SIGNAL(triggered()), q, SLOT(copyFiles()));

    actions[DualPaneWidget::MoveFiles] = new QAction(this);
    actions[DualPaneWidget::MoveFiles]->setShortcut(QKeySequence("F6"));
    actions[DualPaneWidget::MoveFiles]->setEnabled(false);
    connect(actions[DualPaneWidget::MoveFiles], SIGNAL(triggered()), q, SLOT(moveFiles()));

    actions[DualPaneWidget::Rename] = new QAction(this);
    actions[DualPaneWidget::Rename]->setEnabled(false);
    connect(actions[DualPaneWidget::Rename], SIGNAL(triggered()), q, SLOT(rename()));

    actions[DualPaneWidget::Remove] = new QAction(this);
    actions[DualPaneWidget::Remove]->setEnabled(false);
    connect(actions[DualPaneWidget::Remove], SIGNAL(triggered()), q, SLOT(remove()));

    actions[DualPaneWidget::ShowFileInfo] = new QAction(this);
    connect(actions[DualPaneWidget::ShowFileInfo], SIGNAL(triggered()), q, SLOT(showFileInfo()));

    actions[DualPaneWidget::Redo] = new QAction(this);
    actions[DualPaneWidget::Redo]->setEnabled(false);
    connect(actions[DualPaneWidget::Redo], SIGNAL(triggered()), q, SLOT(redo()));
    connect(q, SIGNAL(canRedoChanged(bool)), actions[DualPaneWidget::Redo], SLOT(setEnabled(bool)));

    actions[DualPaneWidget::Undo] = new QAction(this);
    actions[DualPaneWidget::Undo]->setEnabled(false);
    connect(actions[DualPaneWidget::Undo], SIGNAL(triggered()), q, SLOT(undo()));
    connect(q, SIGNAL(canUndoChanged(bool)), actions[DualPaneWidget::Undo], SLOT(setEnabled(bool)));

    actions[DualPaneWidget::Cut] = new QAction(this);
    actions[DualPaneWidget::Cut]->setEnabled(false);
//    connect(actions[DualPaneWidget::Cut], SIGNAL(triggered()), q, SLOT(cut()));

    actions[DualPaneWidget::Copy] = new QAction(this);
    connect(actions[DualPaneWidget::Copy], SIGNAL(triggered()), q, SLOT(copy()));

    actions[DualPaneWidget::Paste] = new QAction(this);
    connect(actions[DualPaneWidget::Paste], SIGNAL(triggered()), q, SLOT(paste()));

    actions[DualPaneWidget::SelectAll] = new QAction(this);
    connect(actions[DualPaneWidget::SelectAll], SIGNAL(triggered()), q, SLOT(selectAll()));

    actions[DualPaneWidget::ShowHiddenFiles] = new QAction(this);
    actions[DualPaneWidget::ShowHiddenFiles]->setCheckable(true);
    connect(actions[DualPaneWidget::ShowHiddenFiles], SIGNAL(triggered(bool)), q, SLOT(showHiddenFiles(bool)));

    viewModeGroup = new QActionGroup(this);

    actions[DualPaneWidget::IconMode] = new QAction(this);
    actions[DualPaneWidget::ColumnMode] = new QAction(this);
    actions[DualPaneWidget::TreeMode] = new QAction(this);
    actions[DualPaneWidget::CoverFlowMode] = new QAction(this);
    actions[DualPaneWidget::EnableDualPane] = new QAction(q);
    actions[DualPaneWidget::VerticalPanels] = new QAction(q);

    actions[DualPaneWidget::IconMode]->setCheckable(true);
    actions[DualPaneWidget::IconMode]->setChecked(true);
    actions[DualPaneWidget::ColumnMode]->setCheckable(true);
    actions[DualPaneWidget::TreeMode]->setCheckable(true);
    actions[DualPaneWidget::CoverFlowMode]->setCheckable(true);
    actions[DualPaneWidget::EnableDualPane]->setCheckable(true);
    actions[DualPaneWidget::VerticalPanels]->setCheckable(true);

    actions[DualPaneWidget::VerticalPanels]->setEnabled(false);

    viewModeGroup->addAction(actions[DualPaneWidget::IconMode]);
    viewModeGroup->addAction(actions[DualPaneWidget::ColumnMode]);
    viewModeGroup->addAction(actions[DualPaneWidget::TreeMode]);
    viewModeGroup->addAction(actions[DualPaneWidget::CoverFlowMode]);

    actions[DualPaneWidget::IconMode]->setData(FileManagerWidget::IconView);
    actions[DualPaneWidget::ColumnMode]->setData(FileManagerWidget::ColumnView);
    actions[DualPaneWidget::TreeMode]->setData(FileManagerWidget::TreeView);
    actions[DualPaneWidget::CoverFlowMode]->setData(FileManagerWidget::CoverFlow);

    connect(actions[DualPaneWidget::IconMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));
    connect(actions[DualPaneWidget::ColumnMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));
    connect(actions[DualPaneWidget::TreeMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));
    connect(actions[DualPaneWidget::CoverFlowMode], SIGNAL(triggered(bool)), SLOT(toggleViewMode(bool)));

    connect(actions[DualPaneWidget::EnableDualPane], SIGNAL(triggered(bool)),
            q, SLOT(setDualPaneModeEnabled(bool)));
    connect(actions[DualPaneWidget::VerticalPanels], SIGNAL(triggered(bool)),
            this, SLOT(toggleOrientation(bool)));

    sortByGroup = new QActionGroup(this);

    actions[DualPaneWidget::SortByName] = new QAction(this);
    actions[DualPaneWidget::SortBySize] = new QAction(this);
    actions[DualPaneWidget::SortByType] = new QAction(this);
    actions[DualPaneWidget::SortByDate] = new QAction(this);
    actions[DualPaneWidget::SortDescendingOrder] = new QAction(this);

    actions[DualPaneWidget::SortByName]->setCheckable(true);
    actions[DualPaneWidget::SortBySize]->setCheckable(true);
    actions[DualPaneWidget::SortByType]->setCheckable(true);
    actions[DualPaneWidget::SortByDate]->setCheckable(true);
    actions[DualPaneWidget::SortDescendingOrder]->setCheckable(true);

    actions[DualPaneWidget::SortByName]->setChecked(true);

    sortByGroup->addAction(actions[DualPaneWidget::SortByName]);
    sortByGroup->addAction(actions[DualPaneWidget::SortBySize]);
    sortByGroup->addAction(actions[DualPaneWidget::SortByType]);
    sortByGroup->addAction(actions[DualPaneWidget::SortByDate]);

    actions[DualPaneWidget::SortByName]->setData(FileManagerWidget::NameColumn);
    actions[DualPaneWidget::SortBySize]->setData(FileManagerWidget::SizeColumn);
    actions[DualPaneWidget::SortByType]->setData(FileManagerWidget::TypeColumn);
    actions[DualPaneWidget::SortByDate]->setData(FileManagerWidget::DateColumn);

    connect(actions[DualPaneWidget::SortByName], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[DualPaneWidget::SortBySize], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[DualPaneWidget::SortByType], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[DualPaneWidget::SortByDate], SIGNAL(triggered(bool)), SLOT(toggleSortColumn(bool)));
    connect(actions[DualPaneWidget::SortDescendingOrder], SIGNAL(triggered(bool)), SLOT(toggleSortOrder(bool)));

    for (int i = 0; i < DualPaneWidget::ActionCount; i++) {
        q->addAction(actions[i]);
    }
}

void DualPaneWidgetPrivate::retranslateUi()
{
    actions[DualPaneWidget::Open]->setText(tr("Open"));
    actions[DualPaneWidget::OpenInTab]->setText(tr("Open in new tab"));
    actions[DualPaneWidget::OpenInWindow]->setText(tr("Open in new window"));

    actions[DualPaneWidget::SelectProgram]->setText(tr("Select program..."));
    actions[DualPaneWidget::NewFolder]->setText(tr("New Folder"));
    actions[DualPaneWidget::CopyFiles]->setText(tr("Copy files"));
    actions[DualPaneWidget::MoveFiles]->setText(tr("Move files"));
    actions[DualPaneWidget::Rename]->setText(tr("Rename"));
    actions[DualPaneWidget::Remove]->setText(tr("Remove"));
    actions[DualPaneWidget::ShowFileInfo]->setText(tr("File info"));

    actions[DualPaneWidget::Redo]->setText(tr("Redo"));
    actions[DualPaneWidget::Undo]->setText(tr("Undo"));
    actions[DualPaneWidget::Cut]->setText(tr("Cut"));
    actions[DualPaneWidget::Copy]->setText(tr("Copy"));
    actions[DualPaneWidget::Paste]->setText(tr("Paste"));
    actions[DualPaneWidget::SelectAll]->setText(tr("Select all"));

    actions[DualPaneWidget::ShowHiddenFiles]->setText(tr("Show hidden files"));

    actions[DualPaneWidget::IconMode]->setText(tr("Icon view"));
    actions[DualPaneWidget::ColumnMode]->setText(tr("Column view"));
    actions[DualPaneWidget::TreeMode]->setText(tr("Tree view"));
    actions[DualPaneWidget::CoverFlowMode]->setText(tr("Cover flow"));
    actions[DualPaneWidget::EnableDualPane]->setText(tr("Enable dual pane"));
    actions[DualPaneWidget::VerticalPanels]->setText(tr("Vertical panels"));

    actions[DualPaneWidget::SortByName]->setText(tr("Sort by name"));
    actions[DualPaneWidget::SortBySize]->setText(tr("Sort by size"));
    actions[DualPaneWidget::SortByType]->setText(tr("Sort by type"));
    actions[DualPaneWidget::SortByDate]->setText(tr("Sort by date"));
    actions[DualPaneWidget::SortDescendingOrder]->setText(tr("Descending order"));
}

FileManagerWidget * DualPaneWidgetPrivate::createPane()
{
    Q_Q(DualPaneWidget);

    FileManagerWidget *pane = new FileManagerWidget(q);

    pane->installEventFilter(q);

    QObject::connect(pane, SIGNAL(currentPathChanged(QString)), q, SIGNAL(currentPathChanged(QString)));
    QObject::connect(pane, SIGNAL(openRequested(QString)), q, SIGNAL(openRequested(QString)));
    QObject::connect(pane, SIGNAL(openNewTabRequested(QStringList)), q, SIGNAL(openNewTabRequested(QStringList)));
    QObject::connect(pane, SIGNAL(openNewWindowRequested(QStringList)), q, SIGNAL(openNewWindowRequested(QStringList)));
    QObject::connect(pane, SIGNAL(canRedoChanged(bool)), q, SIGNAL(canRedoChanged(bool)));
    QObject::connect(pane, SIGNAL(canUndoChanged(bool)), q, SIGNAL(canUndoChanged(bool)));
    QObject::connect(pane, SIGNAL(selectedPathsChanged()), SLOT(onSelectionChanged()));
    QObject::connect(pane, SIGNAL(selectedPathsChanged()), q, SIGNAL(selectedPathsChanged()));
    QObject::connect(pane, SIGNAL(sortingChanged()), q, SIGNAL(sortingChanged()));
    QObject::connect(pane, SIGNAL(viewModeChanged(FileManagerWidget::ViewMode)),
                     q, SIGNAL(viewModeChanged(FileManagerWidget::ViewMode)));

    return pane;
}

void DualPaneWidgetPrivate::createLeftPane()
{
    panes[DualPaneWidget::LeftPane] = createPane();
    splitter->addWidget(panes[DualPaneWidget::LeftPane]);
}

void DualPaneWidgetPrivate::createRightPane()
{
    panes[DualPaneWidget::RightPane] = createPane();

    panes[DualPaneWidget::RightPane]->hide();
    panes[DualPaneWidget::RightPane]->setViewMode(FileManagerWidget::TreeView);
    panes[DualPaneWidget::RightPane]->setAlternatingRowColors(false);
    splitter->addWidget(panes[DualPaneWidget::RightPane]);

    swapPalettes(panes[DualPaneWidget::LeftPane], panes[DualPaneWidget::RightPane]);
}

void DualPaneWidgetPrivate::ensureRightPaneCreated()
{
    if (!panes[DualPaneWidget::RightPane])
        createRightPane();
}

void DualPaneWidgetPrivate::updateViewModeActions()
{
    Q_Q(DualPaneWidget);

    FileManagerWidget::ViewMode mode = q->activeWidget()->viewMode();

    actions[DualPaneWidget::IconMode]->setChecked(mode == FileManagerWidget::IconView);
    actions[DualPaneWidget::ColumnMode]->setChecked(mode == FileManagerWidget::ColumnView);
    actions[DualPaneWidget::TreeMode]->setChecked(mode == FileManagerWidget::TreeView);
    actions[DualPaneWidget::CoverFlowMode]->setChecked(mode == FileManagerWidget::CoverFlow);
}

void DualPaneWidgetPrivate::updateSortActions()
{
    Q_Q(DualPaneWidget);

    FileManagerWidget::Column column = q->activeWidget()->sortingColumn();
    Qt::SortOrder order = q->activeWidget()->sortingOrder();

    for (int i = FileManagerWidget::NameColumn; i < FileManagerWidget::ColumnCount; i++) {
        actions[DualPaneWidget::SortByName + i]->setChecked(column == FileManagerWidget::NameColumn + i);
    }

    actions[DualPaneWidget::SortDescendingOrder]->setChecked(order == Qt::DescendingOrder);
}

void DualPaneWidgetPrivate::openNewTab()
{
    Q_Q(DualPaneWidget);

    QStringList paths = q->selectedPaths();

    if (!paths.isEmpty())
        emit q->openNewTabRequested(paths);
}

void DualPaneWidgetPrivate::openNewWindow()
{
    Q_Q(DualPaneWidget);

    QStringList paths = q->selectedPaths();

    if (!paths.isEmpty())
        emit q->openNewWindowRequested(paths);
}

void DualPaneWidgetPrivate::toggleViewMode(bool toggled)
{
    Q_Q(DualPaneWidget);

    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    if (toggled)
        q->setViewMode((FileManagerWidget::ViewMode)action->data().toInt());
    else
        action->setChecked(true);
}

void DualPaneWidgetPrivate::toggleOrientation(bool toggled)
{
    Q_Q(DualPaneWidget);

    q->setOrientation(toggled ? Qt::Vertical : Qt::Horizontal);
}

void DualPaneWidgetPrivate::toggleSortColumn(bool toggled)
{
    Q_Q(DualPaneWidget);

    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    if (toggled)
        q->setSortingColumn((FileManagerWidget::Column)action->data().toInt());
    else
        action->setChecked(true);
}

void DualPaneWidgetPrivate::toggleSortOrder(bool descending)
{
    Q_Q(DualPaneWidget);

    q->setSortingOrder(descending ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void DualPaneWidgetPrivate::onSelectionChanged()
{
    Q_Q(DualPaneWidget);

    QStringList paths = q->selectedPaths();
    bool enabled = !paths.empty();

    actions[DualPaneWidget::Open]->setEnabled(enabled);
    actions[DualPaneWidget::OpenInTab]->setEnabled(enabled);
    actions[DualPaneWidget::OpenInWindow]->setEnabled(enabled);
    actions[DualPaneWidget::Rename]->setEnabled(enabled);
    actions[DualPaneWidget::Remove]->setEnabled(enabled);
//    actions[DualPaneWidget::Cut]->setEnabled(enabled);
    actions[DualPaneWidget::Copy]->setEnabled(enabled);

    if (!paths.isEmpty()) {
        if (paths.size() == 1) {
            actions[DualPaneWidget::Cut]->setText(tr("Cut \"%1\"").arg(QFileInfo(paths[0]).fileName()));
            actions[DualPaneWidget::Copy]->setText(tr("Copy \"%1\"").arg(QFileInfo(paths[0]).fileName()));
        } else {
            actions[DualPaneWidget::Cut]->setText(tr("Cut %1 items").arg(paths.size()));
            actions[DualPaneWidget::Copy]->setText(tr("Copy %1 items").arg(paths.size()));
        }
    } else {
        actions[DualPaneWidget::Cut]->setText(tr("Cut"));
        actions[DualPaneWidget::Copy]->setText(tr("Copy"));
    }
}

// todo : uncomment when c++0x will be standard
//DualPaneWidget::DualPaneWidget(QWidget *parent) :
//    DualPaneWidget(0, parent)
//{
//}

DualPaneWidget::DualPaneWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new DualPaneWidgetPrivate(this))
{
    Q_D(DualPaneWidget);

    d->activePane = LeftPane;
    d->dualPaneModeEnabled = false;
    d->panes[LeftPane] = 0;
    d->panes[RightPane] = 0;

    d->layout = new QVBoxLayout();
    d->layout->setMargin(0);
    d->layout->setSpacing(0);
    setLayout(d->layout);

    d->splitter = new QSplitter(Qt::Horizontal, this);
    d->splitter->setHandleWidth(5);
    d->layout->addWidget(d->splitter);

    d->createLeftPane();
    d->createActions();
    d->retranslateUi();

    setObjectName(QLatin1String("DualPaneWidget"));
}

DualPaneWidget::~DualPaneWidget()
{
    delete d_ptr;
}

QAction * DualPaneWidget::action(FileManager::DualPaneWidget::Action action) const
{
    Q_D(const DualPaneWidget);

    if (action <= NoAction || action >= ActionCount)
        return 0;

    return d->actions[action];
}

History * DualPaneWidget::history() const
{
    return activeWidget()->history();
}

DualPaneWidget::Pane DualPaneWidget::activePane() const
{
    return d_func()->activePane;
}

void DualPaneWidget::setActivePane(DualPaneWidget::Pane pane)
{
    Q_D(DualPaneWidget);

    if (d->activePane == pane)
        return;

    d->activePane = pane;
    swapPalettes(d->panes[pane], d->panes[(pane + 1)%2]);
    emit activePaneChanged(d->activePane);
    d->updateState();
}

FileManagerWidget * DualPaneWidget::activeWidget() const
{
    Q_D(const DualPaneWidget);

    return d->panes[d->activePane];
}

FileManagerWidget * DualPaneWidget::leftWidget() const
{
    return d_func()->panes[LeftPane];
}

FileManagerWidget * DualPaneWidget::rightWidget() const
{
    DualPaneWidgetPrivate * d = d_ptr;
    d->ensureRightPaneCreated();
    return d_func()->panes[RightPane];
}

QString DualPaneWidget::currentPath() const
{
    return activeWidget()->currentPath();
}

void DualPaneWidget::setCurrentPath(const QString &path)
{
    Q_D(DualPaneWidget);

    activeWidget()->setCurrentPath(path);

    if (dualPaneModeEnabled() && d->panes[RightPane]->currentPath().isEmpty())
        d->panes[RightPane]->setCurrentPath(path);
}

bool DualPaneWidget::dualPaneModeEnabled() const
{
    return d_func()->dualPaneModeEnabled;
}

void DualPaneWidget::setDualPaneModeEnabled(bool on)
{
    Q_D(DualPaneWidget);

    if (d->dualPaneModeEnabled == on)
        return;

    d->dualPaneModeEnabled = on;
    if (on) {
        d->ensureRightPaneCreated();

        d->panes[RightPane]->show();

        if (d->panes[RightPane]->currentPath().isEmpty())
            d->panes[RightPane]->setCurrentPath(d->panes[LeftPane]->currentPath());

        d->panes[LeftPane]->setAlternatingRowColors(false);

    } else {
        if (d->panes[RightPane])
            d->panes[RightPane]->hide();

        setActivePane(LeftPane);
        d->panes[LeftPane]->setAlternatingRowColors(true);
    }

    d->actions[EnableDualPane]->setChecked(on);
    d->actions[CopyFiles]->setEnabled(on);
    d->actions[MoveFiles]->setEnabled(on);
    d->actions[VerticalPanels]->setEnabled(on);

    emit dualPaneModeChanged(on);
}

FileManagerWidget::ViewMode DualPaneWidget::viewMode() const
{
    return activeWidget()->viewMode();
}

Qt::Orientation DualPaneWidget::orientation() const
{
    Q_D(const DualPaneWidget);

    return d->splitter->orientation();
}

void DualPaneWidget::setOrientation(Qt::Orientation orientation)
{
    Q_D(DualPaneWidget);

    if (d->splitter->orientation() == orientation)
        return;

    d->splitter->setOrientation(orientation);
    d->actions[VerticalPanels]->setChecked(orientation == Qt::Vertical);
    emit orientationChanged(orientation);
}

void DualPaneWidget::setViewMode(FileManagerWidget::ViewMode mode)
{
    Q_D(DualPaneWidget);

    activeWidget()->setViewMode(mode);

    d->updateViewModeActions();
}

QStringList DualPaneWidget::selectedPaths() const
{
    return activeWidget()->selectedPaths();
}

FileManagerWidget::Column DualPaneWidget::sortingColumn() const
{
    return activeWidget()->sortingColumn();
}

void DualPaneWidget::setSortingColumn(FileManagerWidget::Column column)
{
    Q_D(DualPaneWidget);

    activeWidget()->setSortingColumn(column);

    d->updateSortActions();
}

Qt::SortOrder DualPaneWidget::sortingOrder() const
{
    return activeWidget()->sortingOrder();
}

void DualPaneWidget::setSortingOrder(Qt::SortOrder order)
{
    Q_D(DualPaneWidget);

    activeWidget()->setSortingOrder(order);
    d->updateSortActions();
}

bool DualPaneWidget::restoreState(const QByteArray &arr)
{
    Q_D(DualPaneWidget);

    if (arr.isEmpty())
        return false;

    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    bool b;
    QByteArray splitterState;
    QByteArray subState;
    s >> b;
    setDualPaneModeEnabled(b);
    s >> splitterState;
    d->splitter->restoreState(splitterState);
    d->actions[VerticalPanels]->setChecked(orientation() == Qt::Vertical);
    s >> subState;
    leftWidget()->restoreState(subState);
    s >> subState;
    if (!subState.isEmpty()) {
        d->ensureRightPaneCreated();
        rightWidget()->restoreState(subState);
    }
    d->updateViewModeActions();

    return true;
}

QByteArray DualPaneWidget::saveState() const
{
    Q_D(const DualPaneWidget);

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << dualPaneModeEnabled();
    s << d->splitter->saveState();
    s << leftWidget()->saveState();
    if (d_func()->panes[RightPane])
        s << rightWidget()->saveState();

    return state;
}

void DualPaneWidget::sync()
{
    Q_D(DualPaneWidget);

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    target->setCurrentPath(source->currentPath());
}

void DualPaneWidget::newFolder()
{
    activeWidget()->newFolder();
}

void DualPaneWidget::open()
{
    activeWidget()->open();
}

void DualPaneWidget::selectProgram()
{
    activeWidget()->selectProgram();
}

void DualPaneWidget::showFileInfo()
{
    activeWidget()->showFileInfo();
}

void DualPaneWidget::remove()
{
    activeWidget()->remove();
}

void DualPaneWidget::rename()
{
    activeWidget()->rename();
}

void DualPaneWidget::copyFiles()
{
    Q_D(DualPaneWidget);

    if (!d->dualPaneModeEnabled)
        return;

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    QStringList files = source->selectedPaths();
    source->fileSystemManager()->copy(files, target->currentPath());
}

void DualPaneWidget::moveFiles()
{
    Q_D(DualPaneWidget);

    if (!d->dualPaneModeEnabled)
        return;

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    QStringList files = source->selectedPaths();
    source->fileSystemManager()->move(files, target->currentPath());
}

void DualPaneWidget::undo()
{
    activeWidget()->undo();
}

void DualPaneWidget::redo()
{
    activeWidget()->redo();
}

void DualPaneWidget::cut()
{
}

void DualPaneWidget::copy()
{
    activeWidget()->copy();
}

void DualPaneWidget::paste()
{
    activeWidget()->paste();
}

void DualPaneWidget::selectAll()
{
    activeWidget()->selectAll();
}

void DualPaneWidget::back()
{
    activeWidget()->back();
}

void DualPaneWidget::forward()
{
    activeWidget()->forward();
}

void DualPaneWidget::up()
{
    activeWidget()->up();
}

void DualPaneWidget::showHiddenFiles(bool show)
{
    Q_D(DualPaneWidget);
    d->panes[LeftPane]->showHiddenFiles(show);
    if (d->panes[RightPane])
        d->panes[RightPane]->showHiddenFiles(show);
}

void DualPaneWidgetPrivate::updateState()
{
    Q_Q(DualPaneWidget);

    if (q->leftWidget()->sortingOrder() != q->rightWidget()->sortingOrder() ||
            q->leftWidget()->sortingColumn() != q->rightWidget()->sortingColumn())
        emit q->sortingChanged();

    if (q->leftWidget()->viewMode() != q->rightWidget()->viewMode())
        emit q->viewModeChanged(q->viewMode());

    emit q->currentPathChanged(q->activeWidget()->currentPath());
}

bool DualPaneWidget::eventFilter(QObject *o, QEvent *e)
{
    Q_D(DualPaneWidget);

    if (e->type() == QEvent::FocusIn) {
        if (o == d->panes[LeftPane]) {
            setActivePane(LeftPane);
        } else if (o == d->panes[RightPane]) {
            setActivePane(RightPane);
        }
        d->updateViewModeActions();
        d->updateSortActions();
    }

    return false;
}
