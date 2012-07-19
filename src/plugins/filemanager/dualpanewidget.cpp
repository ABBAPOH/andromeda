#include "dualpanewidget.h"
#include "dualpanewidget_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QEvent>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"
#include "filesystemmanager.h"

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

    actions[DualPaneWidget::EnableDualPane] = new QAction(q);
    actions[DualPaneWidget::EnableDualPane]->setCheckable(true);
    connect(actions[DualPaneWidget::EnableDualPane], SIGNAL(triggered(bool)), q, SLOT(setDualPaneModeEnabled(bool)));

    actions[DualPaneWidget::VerticalPanels] = new QAction(q);
    actions[DualPaneWidget::VerticalPanels]->setCheckable(true);
    actions[DualPaneWidget::VerticalPanels]->setEnabled(false);
    connect(actions[DualPaneWidget::VerticalPanels], SIGNAL(triggered(bool)), SLOT(toggleOrientation(bool)));

    actions[DualPaneWidget::ToggleActivePane] = new QAction(this);
    actions[DualPaneWidget::ToggleActivePane]->setEnabled(false);
    connect(actions[DualPaneWidget::ToggleActivePane], SIGNAL(triggered()), q, SLOT(toggleActivePane()));

    actions[DualPaneWidget::SyncPanes] = new QAction(this);
    actions[DualPaneWidget::SyncPanes]->setEnabled(false);
    connect(actions[DualPaneWidget::SyncPanes], SIGNAL(triggered()), q, SLOT(syncPanes()));

    actions[DualPaneWidget::SwapPanes] = new QAction(this);
    actions[DualPaneWidget::SwapPanes]->setEnabled(false);
    connect(actions[DualPaneWidget::SwapPanes], SIGNAL(triggered()), q, SLOT(swapPanes()));

    actions[DualPaneWidget::CopyFiles] = new QAction(this);
    actions[DualPaneWidget::CopyFiles]->setShortcut(QKeySequence("F5"));
    actions[DualPaneWidget::CopyFiles]->setEnabled(false);
    connect(actions[DualPaneWidget::CopyFiles], SIGNAL(triggered()), q, SLOT(copyFiles()));

    actions[DualPaneWidget::MoveFiles] = new QAction(this);
    actions[DualPaneWidget::MoveFiles]->setShortcut(QKeySequence("F6"));
    actions[DualPaneWidget::MoveFiles]->setEnabled(false);
    connect(actions[DualPaneWidget::MoveFiles], SIGNAL(triggered()), q, SLOT(moveFiles()));

    for (int i = 0; i < DualPaneWidget::ActionCount; i++) {
        q->addAction(actions[i]);
    }
}

void DualPaneWidgetPrivate::retranslateUi()
{
    actions[DualPaneWidget::EnableDualPane]->setText(tr("Enable dual pane"));
    actions[DualPaneWidget::VerticalPanels]->setText(tr("Vertical panes"));
    actions[DualPaneWidget::ToggleActivePane]->setText(tr("Toggle active pane"));
    actions[DualPaneWidget::SyncPanes]->setText(tr("Sync panes"));
    actions[DualPaneWidget::SwapPanes]->setText(tr("Swap panes"));
    actions[DualPaneWidget::CopyFiles]->setText(tr("Copy files"));
    actions[DualPaneWidget::MoveFiles]->setText(tr("Move files"));
}

FileManagerWidget * DualPaneWidgetPrivate::createPane()
{
    Q_Q(DualPaneWidget);

    FileManagerWidget *pane = new FileManagerWidget(q);

    pane->installEventFilter(q);

    QObject::connect(pane, SIGNAL(currentPathChanged(QString)), q, SIGNAL(currentPathChanged(QString)));
    QObject::connect(pane, SIGNAL(editRequested(QString)), q, SIGNAL(editRequested(QString)));
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
    splitter->addWidget(panes[DualPaneWidget::RightPane]);

    swapPalettes(panes[DualPaneWidget::LeftPane], panes[DualPaneWidget::RightPane]);
}

void DualPaneWidgetPrivate::ensureRightPaneCreated()
{
    if (!panes[DualPaneWidget::RightPane])
        createRightPane();
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

    if (toggled) {
        QAction *action = qobject_cast<QAction*>(sender());
        if (action)
            q->setViewMode((FileManagerWidget::ViewMode)action->data().toInt());
    }
}

void DualPaneWidgetPrivate::toggleOrientation(bool toggled)
{
    Q_Q(DualPaneWidget);

    q->setOrientation(toggled ? Qt::Vertical : Qt::Horizontal);
}

void DualPaneWidgetPrivate::toggleSortColumn(bool toggled)
{
    Q_Q(DualPaneWidget);

    if (toggled) {
        QAction *action = qobject_cast<QAction*>(sender());
        if (action)
            q->setSortingColumn((FileManagerWidget::Column)action->data().toInt());
    }
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
    bool enabled = !paths.empty() && dualPaneModeEnabled;

    actions[DualPaneWidget::CopyFiles]->setEnabled(enabled);
    actions[DualPaneWidget::MoveFiles]->setEnabled(enabled);
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

FileManagerHistory * DualPaneWidget::history() const
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
        d->panes[RightPane]->setAlternatingRowColors(false);

    } else {
        if (d->panes[RightPane])
            d->panes[RightPane]->hide();

        setActivePane(LeftPane);
        d->panes[LeftPane]->setAlternatingRowColors(true);
        d->panes[RightPane]->setAlternatingRowColors(true);
    }

    d->actions[EnableDualPane]->setChecked(on);
    d->actions[VerticalPanels]->setEnabled(on);
    d->actions[ToggleActivePane]->setEnabled(on);
    d->actions[SyncPanes]->setEnabled(on);
    d->actions[SwapPanes]->setEnabled(on);

    QStringList paths = selectedPaths();
    d->actions[CopyFiles]->setEnabled(on && !paths.isEmpty());
    d->actions[MoveFiles]->setEnabled(on && !paths.isEmpty());

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

    if (orientation != Qt::Vertical && orientation != Qt::Horizontal)
        return;

    d->splitter->setOrientation(orientation);
    d->actions[VerticalPanels]->setChecked(orientation == Qt::Vertical);
    emit orientationChanged(orientation);
}

void DualPaneWidget::toggleActivePane()
{
    setActivePane(activePane() == LeftPane ? RightPane : LeftPane);
}

void DualPaneWidget::setViewMode(FileManagerWidget::ViewMode mode)
{
    activeWidget()->setViewMode(mode);
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
    activeWidget()->setSortingColumn(column);
}

Qt::SortOrder DualPaneWidget::sortingOrder() const
{
    return activeWidget()->sortingOrder();
}

void DualPaneWidget::setSortingOrder(Qt::SortOrder order)
{
    activeWidget()->setSortingOrder(order);
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

void DualPaneWidget::clear()
{
    Q_D(DualPaneWidget);

    d->panes[LeftPane]->setFocus();
    d->panes[LeftPane]->clear();

    if (d->panes[RightPane])
        d->panes[RightPane]->clear();
}

void DualPaneWidget::syncPanes()
{
    Q_D(DualPaneWidget);

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    target->setCurrentPath(source->currentPath());

    // sync scroll bar
    int scrollBarValue = source->d_func()->currentView->verticalScrollBar()->value();
    target->d_func()->currentView->verticalScrollBar()->setValue(scrollBarValue);

    // TODO: sync selection too
}

void DualPaneWidget::swapPanes()
{
    Pane pane = activePane();

    FileManagerWidget *left = leftWidget();
    FileManagerWidget *right = rightWidget();

    QString leftPath = left->currentPath();
    QString rightPath = right->currentPath();

    left->setCurrentPath(rightPath);
    right->setCurrentPath(leftPath);

    // TODO: swap and scroll selection too

    setActivePane(pane == LeftPane ? RightPane : LeftPane);
}

void DualPaneWidget::newFolder()
{
    activeWidget()->newFolder();
}

void DualPaneWidget::open()
{
    activeWidget()->open();
}

void DualPaneWidget::edit()
{
    activeWidget()->edit();
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

void DualPaneWidget::moveToTrash()
{
    activeWidget()->moveToTrash();
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

void DualPaneWidget::moveHere()
{
    activeWidget()->moveHere();
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
    }

    return false;
}
