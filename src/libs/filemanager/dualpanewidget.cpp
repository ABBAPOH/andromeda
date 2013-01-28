#include "dualpanewidget.h"
#include "dualpanewidget_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QEvent>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "filemanagerconstants.h"
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
    actions[DualPaneWidget::EnableDualPane]->setObjectName(Constants::Actions::DualPane);
    connect(actions[DualPaneWidget::EnableDualPane], SIGNAL(triggered(bool)), q, SLOT(setDualPaneModeEnabled(bool)));

    actions[DualPaneWidget::VerticalPanels] = new QAction(q);
    actions[DualPaneWidget::VerticalPanels]->setCheckable(true);
    actions[DualPaneWidget::VerticalPanels]->setEnabled(false);
    actions[DualPaneWidget::VerticalPanels]->setObjectName(Constants::Actions::VerticalPanels);
    connect(actions[DualPaneWidget::VerticalPanels], SIGNAL(triggered(bool)), SLOT(toggleOrientation(bool)));

    actions[DualPaneWidget::ToggleActivePane] = new QAction(this);
    actions[DualPaneWidget::ToggleActivePane]->setEnabled(false);
    actions[DualPaneWidget::ToggleActivePane]->setObjectName(Constants::Actions::ToggleActivePane);
    connect(actions[DualPaneWidget::ToggleActivePane], SIGNAL(triggered()), q, SLOT(toggleActivePane()));

    actions[DualPaneWidget::SyncPanes] = new QAction(this);
    actions[DualPaneWidget::SyncPanes]->setEnabled(false);
    actions[DualPaneWidget::SyncPanes]->setObjectName(Constants::Actions::SyncPanes);
    connect(actions[DualPaneWidget::SyncPanes], SIGNAL(triggered()), q, SLOT(syncPanes()));

    actions[DualPaneWidget::SwapPanes] = new QAction(this);
    actions[DualPaneWidget::SwapPanes]->setEnabled(false);
    actions[DualPaneWidget::SwapPanes]->setObjectName(Constants::Actions::SwapPanes);
    connect(actions[DualPaneWidget::SwapPanes], SIGNAL(triggered()), q, SLOT(swapPanes()));

    actions[DualPaneWidget::CopyFiles] = new QAction(this);
    actions[DualPaneWidget::CopyFiles]->setShortcut(QKeySequence("F5"));
    actions[DualPaneWidget::CopyFiles]->setEnabled(false);
    actions[DualPaneWidget::CopyFiles]->setObjectName(Constants::Actions::CopyFiles);
    connect(actions[DualPaneWidget::CopyFiles], SIGNAL(triggered()), q, SLOT(copyFiles()));

    actions[DualPaneWidget::MoveFiles] = new QAction(this);
    actions[DualPaneWidget::MoveFiles]->setShortcut(QKeySequence("F6"));
    actions[DualPaneWidget::MoveFiles]->setEnabled(false);
    actions[DualPaneWidget::MoveFiles]->setObjectName(Constants::Actions::MoveFiles);
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
    QObject::connect(pane, SIGNAL(openRequested(QStringList,Qt::KeyboardModifiers)),
                     q, SIGNAL(openRequested(QStringList, Qt::KeyboardModifiers)));
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
    if (!rightPaneState.isEmpty()) {
        panes[DualPaneWidget::RightPane]->restoreState(rightPaneState);
        rightPaneState.clear();
    }
}

void DualPaneWidgetPrivate::deleteRightPane()
{
    rightPaneState = panes[DualPaneWidget::RightPane]->saveState();
    delete panes[DualPaneWidget::RightPane];
    panes[DualPaneWidget::RightPane] = 0;
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

/*!
    \class FileManager::DualPaneWidget

    \brief DualPaneWidget represents a two-panel file widget.

    DualPaneWidget consists of two FileManagerWidget widgets layouted in a
    splitter. Widgets can be placed both vertically or horizontaly or right
    widget can be hidden, turning this widget into a usual single pane file
    manager widget. Also, DualPaneWidget provides some convenience actions,
    like copying/moving files to another pane, syncing current folder in both
    panes and so on.

    \image html dualpanewidget.png
*/

/*!
    \enum FileManager::DualPaneWidget::Pane
    This enum describes pane.

    \var FileManager::DualPaneWidget::Pane FileManager::DualPaneWidget::LeftPane
    Left (or top) pane.

    \var FileManager::DualPaneWidget::Pane FileManager::DualPaneWidget::RightPane
    Right (or bottom) pane.
*/

/*!
    \enum FileManager::DualPaneWidget::Action
    This enum describes actions available for DualPaneWidget.

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::EnableDualPane
    Toggle right pane's visibility.

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::VerticalPanels
    Toggle panes direction (vertical or horizontal).

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::ToggleActivePane
    Toggle active pane.

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::SyncPanes
    Synchronize current folder in both panes.

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::SwapPanes
    Swap current folders in both panes.

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::CopyFiles
    Copy files from active pane to the other.

    \var FileManager::DualPaneWidget::Action FileManager::DualPaneWidget::MoveFiles
    Move files from active pane to the other.
*/

/*!
    Creates DualPaneWidget with the given \a parent.
*/
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

/*!
    Destroys DualPaneWidget.
*/
DualPaneWidget::~DualPaneWidget()
{
    delete d_ptr;
}

/*!
    Returns action specified by \a action enum.
*/
QAction * DualPaneWidget::action(FileManager::DualPaneWidget::Action action) const
{
    Q_D(const DualPaneWidget);

    if (action <= NoAction || action >= ActionCount)
        return 0;

    return d->actions[action];
}

/*!
    Returns active pane's history.
*/
FileManagerHistory * DualPaneWidget::history() const
{
    return activeWidget()->history();
}

/*!
    \property DualPaneWidget::activePane.
    This property holds which pane is active at the current moment.
*/
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

/*!
    Returns pointer to the currently active widget.
*/
FileManagerWidget * DualPaneWidget::activeWidget() const
{
    Q_D(const DualPaneWidget);

    return d->panes[d->activePane];
}

/*!
    Returns pointer to the left (or top, if orientation is Qt::Vertical) widget.
*/
FileManagerWidget * DualPaneWidget::leftWidget() const
{
    return d_func()->panes[LeftPane];
}

/*!
    Returns pointer to the right (or bottom, if orientation is Qt::Vertical) widget.

    \note If DualPaneWidget::dualPaneModeEnabled is false, this function will
    return zero pointer.
*/
FileManagerWidget * DualPaneWidget::rightWidget() const
{
    Q_D(const DualPaneWidget);
    return d->panes[RightPane];
}

/*!
    \property DualPaneWidget::currentPath
    This property holds active widget's path.

    \sa FileManagerWidget::currentPath
*/
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

/*!
    \fn void DualPaneWidget::currentPathChanged(const QString &path)
    This signal is emitted when DualPaneWidget::currentPath property is changed.
*/

/*!
    \property DualPaneWidget::dualPaneModeEnabled
    This property holds if right widget is visible, or not.

    \note To save memory, right widget only exists if this property is set to
    true. If not, only state of the widget is stored in memory.
*/
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
        d->createRightPane();

        d->panes[RightPane]->show();

        if (d->panes[RightPane]->currentPath().isEmpty())
            d->panes[RightPane]->setCurrentPath(d->panes[LeftPane]->currentPath());

        d->panes[LeftPane]->setAlternatingRowColors(false);
        d->panes[RightPane]->setAlternatingRowColors(false);

    } else {
        setActivePane(LeftPane);
        d->panes[LeftPane]->setAlternatingRowColors(true);
        d->deleteRightPane();
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

/*!
    \fn void DualPaneWidget::dualPaneModeChanged(bool enabled)
    This signal is emitted when DualPaneWidget::dualPaneModeEnabled property is changed.
*/

/*!
    \property DualPaneWidget::orientation
    This property holds panes' orientation. Default value is Qt::Horizontal.
*/
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

/*!
    \fn void DualPaneWidget::orientationChanged(Qt::Orientation orientation)
    This signal is emitted when DualPaneWidget::orientation property is changed.
*/

/*!
    \property DualPaneWidget::selectedPaths
    This property holds active widget's selected paths.

    \sa FileManagerWidget::selectedPaths
*/
QStringList DualPaneWidget::selectedPaths() const
{
    return activeWidget()->selectedPaths();
}

/*!
    \fn void DualPaneWidget::selectedPathsChanged()
    This signal is emitted when DualPaneWidget::selectedPaths property is changed.
*/

/*!
    \property DualPaneWidget::sortingColumn
    This property holds active widget's sorting column.

    \sa FileManagerWidget::sortingColumn
*/
FileManagerWidget::Column DualPaneWidget::sortingColumn() const
{
    return activeWidget()->sortingColumn();
}

void DualPaneWidget::setSortingColumn(FileManagerWidget::Column column)
{
    activeWidget()->setSortingColumn(column);
}

/*!
    \property DualPaneWidget::sortingOrder
    This property holds active widget's sorting order.

    \sa FileManagerWidget::sortingColumn
*/
Qt::SortOrder DualPaneWidget::sortingOrder() const
{
    return activeWidget()->sortingOrder();
}

void DualPaneWidget::setSortingOrder(Qt::SortOrder order)
{
    activeWidget()->setSortingOrder(order);
}

/*!
    \fn void DualPaneWidget::sortingChanged()
    This signal is emitted when DualPaneWidget::sortingColumn or
    DualPaneWidget::sortingOrder properties are changed.
*/

/*!
    \property DualPaneWidget::viewMode
    This property holds active widget's view mode.

    \sa FileManagerWidget::viewMode
*/
FileManagerWidget::ViewMode DualPaneWidget::viewMode() const
{
    return activeWidget()->viewMode();
}

void DualPaneWidget::setViewMode(FileManagerWidget::ViewMode mode)
{
    activeWidget()->setViewMode(mode);
}

/*!
    \fn void DualPaneWidget::viewModeChanged(FileManagerWidget::ViewMode mode)
    This signal is emitted when DualPaneWidget::viewMode property is changed.
*/

/*!
    Toggles active pane.
*/
void DualPaneWidget::toggleActivePane()
{
    setActivePane(activePane() == LeftPane ? RightPane : LeftPane);
}

/*!
    Restores widget from \a state byte array.
*/
bool DualPaneWidget::restoreState(const QByteArray &state)
{
    Q_D(DualPaneWidget);

    if (state.isEmpty())
        return false;

    QByteArray tmp = state;
    QDataStream s(&tmp, QIODevice::ReadOnly);

    bool b;
    QByteArray splitterState;
    QByteArray leftPaneState;
    s >> b;
    s >> splitterState;
    s >> leftPaneState;
    s >> d->rightPaneState;

    setDualPaneModeEnabled(b);
    d->splitter->restoreState(splitterState);
    d->actions[VerticalPanels]->setChecked(orientation() == Qt::Vertical);
    leftWidget()->restoreState(leftPaneState);

    return true;
}

/*!
    Stores widget to a byte array.
*/
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

/*!
    Clears both panes and sets focus to the left pane.

    \sa FileManagerWidget::clear()
*/
void DualPaneWidget::clear()
{
    Q_D(DualPaneWidget);

    d->panes[LeftPane]->setFocus();
    d->panes[LeftPane]->clear();

    if (d->panes[RightPane])
        d->panes[RightPane]->clear();
}

/*!
    Synchronizes both panes, i.e. sets current folder same in both panes (inactive
    widget's current folder is set to active widget's folder).
*/
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

/*!
    Swaps panes' current folders (exchanges current folder in both panes).
*/
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

/*!
    Copies files selected in an active pane to the other pane.
*/
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

/*!
    Moves files selected in an active pane to the other pane.
*/
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
