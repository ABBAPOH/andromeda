#include "fileexplorerwidget.h"

#include <QtGui/QAction>
#include <QtGui/QLabel>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

#include <Widgets/MiniSplitter>

#include <GuiSystem/constants.h>
#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

namespace FileManager
{

class FileExplorerWidgetPrivate
{
    Q_DECLARE_PUBLIC(FileExplorerWidget)
public:
    explicit FileExplorerWidgetPrivate(FileExplorerWidget *qq) : q_ptr(qq) {}

    void init();
    void retranslateUi();

public:
    MiniSplitter *splitter;
    DualPaneWidget *dualPane;
    NavigationPanel *panel;

    QToolBar *statusBar;
    QLabel *statusLabel;
    QWidget *widget;

    QAction *showLeftPanelAction;
    QAction *showStatusBarAction;

private:
    FileExplorerWidget *q_ptr;
};

static const quint32 fileExplorerMagic = 0xff3130ff; // "?10?"
static const quint8 fileExplorerVersion = 1;

} // namespace FileManager

using namespace FileManager;

void FileExplorerWidgetPrivate::init()
{
    Q_Q(FileExplorerWidget);

    showLeftPanelAction = new QAction(q);
    showLeftPanelAction->setCheckable(true);
    showLeftPanelAction->setChecked(true);
    showLeftPanelAction->setObjectName(Constants::Actions::ShowLeftPanel);

    q->connect(showLeftPanelAction, SIGNAL(triggered(bool)), q, SLOT(setPanelVisible(bool)));
    q->addAction(showLeftPanelAction);

    showStatusBarAction = new QAction(q);
    showStatusBarAction->setCheckable(true);
    showStatusBarAction->setChecked(true);
    showStatusBarAction->setObjectName(Constants::Actions::ShowStatusBar);

    q->connect(showStatusBarAction, SIGNAL(triggered(bool)), q, SLOT(setStatusBarVisible(bool)));
    q->addAction(showStatusBarAction);

    QVBoxLayout *layout = new QVBoxLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    splitter = new MiniSplitter(q);
    layout->addWidget(splitter);

    panel = new NavigationPanel(splitter);

    widget = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(widget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    dualPane = new DualPaneWidget(widget);
    dualPane->setFocus();
    dualPane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightLayout->addWidget(dualPane);

    statusBar = new QToolBar(widget);
    statusBar->addAction(showLeftPanelAction);
    statusBar->addAction(dualPane->action(DualPaneWidget::EnableDualPane));

    statusLabel = new QLabel(statusBar);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    statusLabel->setFont(statusBar->font());
    statusBar->addWidget(statusLabel);
    rightLayout->addWidget(statusBar);

    splitter->addWidget(panel);
    splitter->addWidget(widget);

    q->connect(dualPane, SIGNAL(selectedPathsChanged()), q, SLOT(onSelectedPathsChanged()));

    retranslateUi();
}

void FileExplorerWidgetPrivate::retranslateUi()
{
    Q_Q(FileExplorerWidget);

    showLeftPanelAction->setText(FileExplorerWidget::tr("Show left panel"));
    showLeftPanelAction->setToolTip(FileExplorerWidget::tr("Shows or hides left panel"));
    showLeftPanelAction->setWhatsThis(FileExplorerWidget::tr("Shows or hides left panel"));

    showStatusBarAction->setText(FileExplorerWidget::tr("Show status bar"));
    showStatusBarAction->setToolTip(FileExplorerWidget::tr("Shows or hides status bar"));
    showStatusBarAction->setWhatsThis(FileExplorerWidget::tr("Shows or hides status bar"));

    q->onSelectedPathsChanged();
}

/*!
    \class FileManager::FileExplorerWidget
    \brief FileExplorerWidget represents Explorer(Dolphin, Finder)-like file
    browser widget.

    This class consists of a file browser (which can be in a single or dual
    pane mode) and a navigation panel.

    \image html fileexplorerwidget.png
*/

/*!
    Creates FileExplorerWidget with the given \a parent.
*/
FileExplorerWidget::FileExplorerWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new FileExplorerWidgetPrivate(this))
{
    Q_D(FileExplorerWidget);
    d->init();
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

    d->init();

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

    This signal is emitted when FileExplorerWidget::panelVisible property is changed.

    \note this signal is NOT emited when panel is hidden via QWidget::setVisible call.
*/

/*!
    \property FileExplorerWidget::statusBarVisible
    Holds whether status bars is visible, or not.
*/
bool FileExplorerWidget::isStatusBarVisible() const
{
    Q_D(const FileExplorerWidget);

    return !d->statusBar->isHidden();
}

void FileExplorerWidget::setStatusBarVisible(bool visible)
{
    if (isStatusBarVisible() == visible)
        return;

    Q_D(FileExplorerWidget);
    d->statusBar->setVisible(visible);
    d->showStatusBarAction->setChecked(visible);
    emit statusBarVisibleChanged(visible);
}

/*!
    \fn void FileExplorerWidget::statusBarVisibleChanged(bool visible)

    This signal is emitted when FileExplorerWidget::statusBarVisible property
    is changed.

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

QToolBar * FileExplorerWidget::statusBar() const
{
    Q_D(const FileExplorerWidget);

    return d->statusBar;
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
    Returns pointer to the action that can be used to control status bar's visibility.
*/
QAction * FileExplorerWidget::showStatusBarAction() const
{
    Q_D(const FileExplorerWidget);

    return d->showStatusBarAction;
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
    bool panelVisible, statusBarVisible;
    QByteArray splitterState, widgetState;

    s >> magic;
    if (magic != fileExplorerMagic)
        return false;

    s >> version;
    if (version != fileExplorerVersion)
        return false;

    s >> panelVisible;
    s >> statusBarVisible;
    s >> splitterState;
    s >> widgetState;

    setPanelVisible(panelVisible);
    setStatusBarVisible(statusBarVisible);
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
    s << isStatusBarVisible();
    s << d->splitter->saveState();
    s << d->dualPane->saveState();

    return state;
}

/*!
    \internal
*/
void FileExplorerWidget::onSelectedPathsChanged()
{
    Q_D(FileExplorerWidget);

    QStringList paths = d->dualPane->selectedPaths();
    int count = paths.count();
    QString text = count == 1 ? tr("Selected 1 object") : tr("Selected %1 objects").arg(count);
    d->statusLabel->setText(text);
}
