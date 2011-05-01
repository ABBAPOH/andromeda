#include "dualpanewidget.h"

#include <QtCore/QEvent>
#include <QtGui/QHBoxLayout>

#include "filemanagerwidget.h"
#include "filesystemundomanager.h"

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
}

DualPaneWidget::~DualPaneWidget()
{
    delete d_ptr;
}

CorePlugin::History * DualPaneWidget::activeHistory() const
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

FileManagerWidget * DualPaneWidget::rigthWidget() const
{
    return d_func()->panes[LeftPane];
}

QString DualPaneWidget::currentPath() const
{
    return activeWidget()->currentPath();
}

void DualPaneWidget::setCurrentPath(const QString &path)
{
    activeWidget()->setCurrentPath(path);
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
    } else {
        d->panes[RightPane]->hide();
        setActivePane(LeftPane);
    }
}

void DualPaneWidget::back()
{
    activeWidget()->back();
}

void DualPaneWidget::forward()
{
    activeWidget()->forward();
}

void DualPaneWidget::copy()
{
    Q_D(DualPaneWidget);

    if (!d->dualPaneModeEnabled)
        return;

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    QStringList files = source->selectedPaths();
    CopyCommand * command = new CopyCommand();
    command->setDestination(target->currentPath());
    for (int i = 0; i < files.size(); i++) {
        QString path = files[i];
        command->appendSourcePath(path);
    }

    source->undoManager()->undoStack()->push(command);
}

void DualPaneWidget::move()
{
    Q_D(DualPaneWidget);

    if (!d->dualPaneModeEnabled)
        return;

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    QStringList files = source->selectedPaths();
    MoveCommand * command = new MoveCommand();
    command->setDestination(target->currentPath());
    for (int i = 0; i < files.size(); i++) {
        QString path = files[i];
        command->appendSourcePath(path);
    }

    source->undoManager()->undoStack()->push(command);
}

void DualPaneWidget::sync()
{
    Q_D(DualPaneWidget);

    FileManagerWidget *source = activeWidget();
    FileManagerWidget *target = d->activePane == LeftPane ? d->panes[RightPane] : d->panes[LeftPane];

    target->setCurrentPath(source->currentPath());
}

bool DualPaneWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DualPaneWidget);

    if (event->type() == QEvent::FocusIn) {
        if (watched == d->panes[LeftPane] && d->activePane != LeftPane) {
            qDebug("active left");
            d->activePane = LeftPane;
            emit activePaneChanged(d->activePane);
        }
        if (watched == d->panes[RightPane] && d->activePane != RightPane) {
            qDebug("active right");
            d->activePane = RightPane;
            emit activePaneChanged(d->activePane);
        }

        emit currentPathChanged(activeWidget()->currentPath());
    }
    return false;
}
