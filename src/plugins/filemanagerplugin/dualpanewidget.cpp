#include "dualpanewidget.h"

#include <QtCore/QBuffer>
#include <QtCore/QEvent>
#include <QtGui/QHBoxLayout>

#include "filemanagerwidget.h"
#include "filesystemmanager.h"

namespace FileManagerPlugin {

class DualPaneWidgetPrivate
{
public:
    DualPaneWidget::Pane activePane;
    bool dualPaneModeEnabled;
    FileManagerWidget *panes[2];
    QHBoxLayout *layout;
    FileManagerWidget::ViewMode viewMode;
};

}

using namespace GuiSystem;
using namespace FileManagerPlugin;

void swapPalettes(QWidget *active, QWidget *inactive)
{
    QPalette palette = inactive->palette(); // it is active palette:)
    active->setPalette(palette);
    QColor color = palette.color(QPalette::Disabled, QPalette::Base);
    palette.setColor(QPalette::Active, QPalette::Base, color);
    inactive->setPalette(palette);
}

// todo : uncomment when c++0x will be standard
//DualPaneWidget::DualPaneWidget(QWidget *parent) :
//    DualPaneWidget(0, parent)
//{
//}

DualPaneWidget::DualPaneWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new DualPaneWidgetPrivate)
{
    Q_D(DualPaneWidget);

    d->activePane = LeftPane;
    d->dualPaneModeEnabled = false;
    d->panes[LeftPane] = 0;
    d->panes[RightPane] = 0;
    d->viewMode = FileManagerWidget::IconView;

    d->layout = new QHBoxLayout();
    d->layout->setMargin(0);
    d->layout->setSpacing(3);
    setLayout(d->layout);

    createLeftPane();

    setObjectName(QLatin1String("DualPaneWidget"));
}

DualPaneWidget::~DualPaneWidget()
{
    delete d_ptr;
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
    updateState();
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
    const_cast<DualPaneWidget*>(this)->ensureRightPaneCreated();
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

    d->dualPaneModeEnabled = on;
    if (on) {
        ensureRightPaneCreated();

        d->panes[RightPane]->show();
        d->panes[LeftPane]->setViewMode(FileManagerWidget::TableView);
        if (d->panes[RightPane]->currentPath().isEmpty())
            d->panes[RightPane]->setCurrentPath(d->panes[LeftPane]->currentPath());
    } else {
        if (d->panes[RightPane])
            d->panes[RightPane]->hide();
        d->panes[LeftPane]->setViewMode(d->viewMode);
        setActivePane(LeftPane);
    }
}

FileManagerWidget::ViewMode DualPaneWidget::viewMode() const
{
    return d_func()->viewMode;
}

void DualPaneWidget::setFileSystemManager(FileSystemManager *manager)
{
    Q_D(DualPaneWidget);

    d->panes[LeftPane]->setFileSystemManager(manager);
    if (d->panes[RightPane])
        d->panes[RightPane]->setFileSystemManager(manager);
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

bool DualPaneWidget::restoreState(const QByteArray &state)
{
    if (state.isEmpty())
        return false;

    QByteArray data = state;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QDataStream s(&buffer);
    bool b;
    QByteArray subState;
    s >> b;
    setDualPaneModeEnabled(b);
    s >> subState;
    leftWidget()->restoreState(subState);
    s >> subState;
    if (!subState.isEmpty()) {
        ensureRightPaneCreated();
        rightWidget()->restoreState(subState);
    }

    return true;
}

QByteArray DualPaneWidget::saveState()
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    QDataStream s(&buffer);
    s << dualPaneModeEnabled();
    s << leftWidget()->saveState();
    if (d_func()->panes[RightPane])
        s << rightWidget()->saveState();

    return buffer.data();
}

void DualPaneWidget::setViewMode(FileManagerWidget::ViewMode mode)
{
    Q_D(DualPaneWidget);

    d->viewMode = mode;
    if (!d->dualPaneModeEnabled) {
        d->panes[LeftPane]->setViewMode(mode);
    }
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
    activeWidget()->showHiddenFiles(show);
}

void DualPaneWidget::updateState()
{
    if (leftWidget()->sortingOrder() != rightWidget()->sortingOrder() ||
            leftWidget()->sortingColumn() != rightWidget()->sortingColumn())
        emit sortingChanged();

    if (leftWidget()->viewMode() != rightWidget()->viewMode())
        emit viewModeChanged(viewMode());

    emit currentPathChanged(activeWidget()->currentPath());
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

FileManagerWidget * DualPaneWidget::createPane()
{
    FileManagerWidget *pane = new FileManagerWidget(this);

    pane->installEventFilter(this);

    connect(pane, SIGNAL(currentPathChanged(QString)), SIGNAL(currentPathChanged(QString)));
    connect(pane, SIGNAL(openRequested(QString)), SIGNAL(openRequested(QString)));
    connect(pane, SIGNAL(canRedoChanged(bool)), SIGNAL(canRedoChanged(bool)));
    connect(pane, SIGNAL(canUndoChanged(bool)), SIGNAL(canUndoChanged(bool)));
    connect(pane, SIGNAL(selectedPathsChanged()), SIGNAL(selectedPathsChanged()));
    connect(pane, SIGNAL(sortingChanged()), SIGNAL(sortingChanged()));
    connect(pane, SIGNAL(viewModeChanged(FileManagerWidget::ViewMode)), SIGNAL(viewModeChanged(FileManagerWidget::ViewMode)));

    return pane;
}

void FileManagerPlugin::DualPaneWidget::createLeftPane()
{
    Q_D(DualPaneWidget);

    d->panes[LeftPane] = createPane();
    d->layout->addWidget(d->panes[LeftPane]);
}

void DualPaneWidget::createRightPane()
{
    Q_D(DualPaneWidget);

    d->panes[RightPane] = createPane();

    d->panes[RightPane]->hide();
    d->panes[RightPane]->setViewMode(FileManagerWidget::TableView);
    d->panes[RightPane]->setFileSystemManager(d->panes[LeftPane]->fileSystemManager());
    d->layout->addWidget(d->panes[RightPane]);

    swapPalettes(d->panes[LeftPane], d->panes[RightPane]);
}

void DualPaneWidget::ensureRightPaneCreated()
{
    if (!d_func()->panes[RightPane])
        createRightPane();
}
