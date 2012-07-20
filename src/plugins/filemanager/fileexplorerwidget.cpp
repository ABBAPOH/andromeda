#include "fileexplorerwidget.h"

#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>

#include <widgets/minisplitter.h>

#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

namespace FileManager
{

class FileExplorerWidgetPrivate
{
    Q_DECLARE_PUBLIC(FileExplorerWidget)
public:
    explicit FileExplorerWidgetPrivate(FileExplorerWidget *qq);

    void retranslateUi();

public:
    MiniSplitter *splitter;
    DualPaneWidget *dualPane;
    NavigationPanel *panel;

    QAction *showLeftPanelAction;

private:
    FileExplorerWidget *q_ptr;
};

static const quint32 fileExplorerMagic = 0xff3130ff; // "?10?"
static const quint8 fileExplorerVersion = 1;

} // namespace FileManager

using namespace FileManager;

FileExplorerWidgetPrivate::FileExplorerWidgetPrivate(FileExplorerWidget *qq) :
    q_ptr(qq)
{
    Q_Q(FileExplorerWidget);

    QVBoxLayout *layout = new QVBoxLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    splitter = new MiniSplitter(q);
    layout->addWidget(splitter);

    dualPane = new DualPaneWidget(splitter);
    dualPane->setFocus();

    panel = new NavigationPanel(splitter);

    splitter->addWidget(panel);
    splitter->addWidget(dualPane);

    showLeftPanelAction = new QAction(q);
    showLeftPanelAction->setCheckable(true);
    showLeftPanelAction->setChecked(true);

    q->connect(showLeftPanelAction, SIGNAL(triggered(bool)), q, SLOT(setPanelVisible(bool)));
    q->addAction(showLeftPanelAction);

    retranslateUi();
}

void FileExplorerWidgetPrivate::retranslateUi()
{
    showLeftPanelAction->setText(FileExplorerWidget::tr("Show left panel"));
    showLeftPanelAction->setToolTip(FileExplorerWidget::tr("Shows or hides left panel"));
    showLeftPanelAction->setWhatsThis(FileExplorerWidget::tr("Shows or hides left panel"));
}


/*!
    \class FileManager::FileExplorerWidget
    \brief FileExplorerWidget represents Explorer(Dolphin, Finder)-like file
    browser widget.

    This class consists of a file browser (which can be in a single or dual
    pane mode) and a navigation panel.
*/

/*!
    Creates FileExplorerWidget with the given \a parent.
*/
FileExplorerWidget::FileExplorerWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileExplorerWidgetPrivate(this))
{
    Q_D(FileExplorerWidget);
    d->panel->setModel(new NavigationModel(this));
}

/*!
    Creates FileExplorerWidget with the given \a parent and \a model.

    This constructor doesn't creates a NavigationModel, but uses given one.
    This can be useful if you need to share models between different panels.
*/
FileExplorerWidget::FileExplorerWidget(NavigationModel *model, QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileExplorerWidgetPrivate(this))
{
    Q_D(FileExplorerWidget);

    if (model)
        d->panel->setModel(model);
    else
        d->panel->setModel(new NavigationModel(this));
}

/*!
    Destroys FileExplorerWidget.
*/
FileManager::FileExplorerWidget::~FileExplorerWidget()
{
    delete d_ptr;
}

/*!
    \property FileExplorerWidget::panelVisible
    Holds whether left panel is visible, or not.
*/
bool FileExplorerWidget::isPanelVisible() const
{
    Q_D(const FileExplorerWidget);

    return !d->panel->isHidden();
}

void FileExplorerWidget::setPanelVisible(bool visible)
{
    if (isPanelVisible() == visible)
        return;

    Q_D(FileExplorerWidget);
    d->panel->setVisible(visible);
    d->showLeftPanelAction->setChecked(visible);
    emit panelVisibleChanged(visible);
}

/*!
    \fn void FileExplorerWidget::panelVisibleChanged(bool visible)

    This signal is emitted when FileExplorerWidget::panelVisible is changed.

    \note this signal is NOT emited when panel is hidden via QWidget::setVisible call.
*/

/*!
    Returns pointer to the file browser widget.
*/
DualPaneWidget * FileExplorerWidget::dualPane() const
{
    Q_D(const FileExplorerWidget);

    return d->dualPane;
}

/*!
    Returns pointer to the left panel.
*/
NavigationPanel * FileExplorerWidget::panel() const
{
    Q_D(const FileExplorerWidget);

    return d->panel;
}

/*!
    Returns pointer to the splitter.
*/
QSplitter * FileExplorerWidget::splitter() const
{
    Q_D(const FileExplorerWidget);

    return d->splitter;
}

/*!
    Returns pointer to the action that can be used to control left panel's visibility.
*/
QAction * FileExplorerWidget::showLeftPanelAction() const
{
    Q_D(const FileExplorerWidget);

    return d->showLeftPanelAction;
}

/*!
    Restores widget's state. Returns true on success.
*/
bool FileExplorerWidget::restoreState(const QByteArray &arr)
{
    Q_D(FileExplorerWidget);

    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    bool ok = true;
    quint32 magic;
    quint8 version;
    QByteArray splitterState, widgetState;
    bool visible;

    s >> magic;
    if (magic != fileExplorerMagic)
        return false;

    s >> version;
    if (version != fileExplorerVersion)
        return false;

    s >> visible;
    s >> splitterState;
    s >> widgetState;

    setPanelVisible(visible);
    ok |= d->splitter->restoreState(splitterState);
    ok |= d->dualPane->restoreState(widgetState);

    return ok;
}

/*!
    Stores widget's state to a byte array.
*/
QByteArray FileExplorerWidget::saveState() const
{
    Q_D(const FileExplorerWidget);

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << fileExplorerMagic;
    s << fileExplorerVersion;
    s << isPanelVisible();
    s << d->splitter->saveState();
    s << d->dualPane->saveState();

    return state;
}
