#include "dualpanewidget.h"

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
};

}

using namespace FileManagerPlugin;

DualPaneWidget::DualPaneWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new DualPaneWidgetPrivate)
{
    Q_D(DualPaneWidget);

    d->activePane = LeftPane;
    d->dualPaneModeEnabled = false;
    d->panes[LeftPane] = new FileManagerWidget(this);
    d->panes[RightPane] = new FileManagerWidget(this);
    d->panes[RightPane]->hide();

    d->layout = new QHBoxLayout();
    d->layout->addWidget(d->panes[LeftPane]);
    d->layout->addWidget(d->panes[RightPane]);
    d->layout->setMargin(0);
    d->layout->setSpacing(3);

    setLayout(d->layout);

    d->panes[LeftPane]->installEventFilter(this);
    d->panes[RightPane]->installEventFilter(this);

    connect(d->panes[LeftPane], SIGNAL(currentPathChanged(QString)), SIGNAL(currentPathChanged(QString)));
    connect(d->panes[RightPane], SIGNAL(currentPathChanged(QString)), SIGNAL(currentPathChanged(QString)));

    connect(d->panes[LeftPane], SIGNAL(openRequested(QString)), SIGNAL(openRequested(QString)));
    connect(d->panes[RightPane], SIGNAL(openRequested(QString)), SIGNAL(openRequested(QString)));
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
// TODO: implement
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
        d->panes[RightPane]->show();
        if (d->panes[RightPane]->currentPath().isEmpty())
            d->panes[RightPane]->setCurrentPath(d->panes[LeftPane]->currentPath());
    } else {
        d->panes[RightPane]->hide();
        setActivePane(LeftPane);
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

void DualPaneWidget::copyFiles()
{
    Q_D(DualPaneWidget);

    if (!d->dualPaneModeEnabled)
        return;

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    QStringList files = source->selectedPaths();
    source->fileSystemManager()->copyFiles(files, target->currentPath());
}

void DualPaneWidget::moveFiles()
{
    Q_D(DualPaneWidget);

    if (!d->dualPaneModeEnabled)
        return;

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    QStringList files = source->selectedPaths();
    source->fileSystemManager()->moveFiles(files, target->currentPath());
}

void DualPaneWidget::undo()
{
    activeWidget()->undo();
}

void DualPaneWidget::redo()
{
    activeWidget()->redo();
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

bool DualPaneWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DualPaneWidget);

    if (event->type() == QEvent::FocusIn) {
        if (watched == d->panes[LeftPane] && d->activePane != LeftPane) {
            d->activePane = LeftPane;
            emit activePaneChanged(d->activePane);
        }
        if (watched == d->panes[RightPane] && d->activePane != RightPane) {
            d->activePane = RightPane;
            emit activePaneChanged(d->activePane);
        }

        emit currentPathChanged(activeWidget()->currentPath());
    }

    return false;
}
