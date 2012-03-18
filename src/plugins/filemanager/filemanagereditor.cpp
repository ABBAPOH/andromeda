#include "filemanagereditor.h"
#include "filemanagereditor_p.h"

#include "filemanagerconstants.h"
#include "filesystemmanager.h"
#include "filesystemmodel.h"
#include "fileinfodialog.h"
#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

#include <QtCore/QDataStream>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QFileIconProvider>
#include <QtGui/QResizeEvent>

#include <extensionsystem/pluginmanager.h>
#include <widgets/minisplitter.h>

#include <core/constants.h>

using namespace GuiSystem;
using namespace FileManager;

FileManagerHistory::FileManagerHistory(QObject *parent) :
    AbstractHistory(parent),
    m_widget(0)
{
}

void FileManagerHistory::clear()
{
    // TODO: implement
}

int FileManagerHistory::count() const
{
    // TODO: implement
    return -1;
}

int FileManagerHistory::currentItemIndex() const
{
    if (m_widget->activePane() == DualPaneWidget::LeftPane)
        return m_widget->leftWidget()->history()->currentItemIndex(); // 0, 1, 2
    else
        return -m_widget->rightWidget()->history()->currentItemIndex() - 2; // -2, -3, -4
}

void FileManagerHistory::setCurrentItemIndex(int index)
{
    DualPaneWidget::Pane pane;
    if (index < -1) {
        pane = DualPaneWidget::RightPane;
        index = -index - 2;
    } else {
        pane = DualPaneWidget::LeftPane;
    }
    m_widget->setActivePane(pane);
    m_widget->activeWidget()->history()->setCurrentItemIndex(index);
}

HistoryItem FileManagerHistory::itemAt(int index) const
{
    // TODO: implement
    return HistoryItem();
}

/*!
  \class FileManagerEditor
*/
FileManagerEditor::FileManagerEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_settings(new QSettings(this)),
    ignoreSignals(false)
{
    setupUi();
    setupConnections();
    createActions();

    m_history = new FileManagerHistory(this);
    m_history->setDualPaneWidget(m_widget);
}

/*!
  \reimp
*/
AbstractEditor::Capabilities FileManagerEditor::capabilities() const
{
    return HasHistory;
}

/*!
  \reimp
*/
QUrl FileManagerEditor::url() const
{
    return QUrl::fromLocalFile(m_widget->currentPath());
}

/*!
  \reimp
*/
void FileManagerEditor::open(const QUrl &url)
{
    emit loadStarted();
    QString path = url.toLocalFile();
#ifdef Q_OS_WIN
    if (path == QLatin1String("/"))
        path.clear();
#endif
    m_widget->setCurrentPath(path);
    emit loadFinished(true);
}

/*!
  \reimp
*/
AbstractHistory * FileManagerEditor::history() const
{
    return m_history;
}

/*!
  \reimp
*/
QIcon FileManagerEditor::icon() const
{
    return QFileIconProvider().icon(QFileInfo(m_widget->currentPath()));
}

/*!
  \reimp
*/
QString FileManagerEditor::title() const
{
    QString path = m_widget->currentPath();
    if (path.endsWith(QLatin1Char('/')))
        path = path.left(path.length() - 1);

    if (path.isEmpty())
        return QLatin1String("/");

    QFileInfo fi(path);
    if (fi.exists()) {
        QString result = fi.fileName();
        if (result.isEmpty())
            result = fi.filePath();
        return result;
    }
    return QString();
}

/*!
  \reimp
*/
QString FileManagerEditor::windowTitle() const
{
    return title();
}

/*!
  \internal

  Restores FileManagerEditor's default settings.
*/
void FileManagerEditor::restoreDefaults()
{
    bool showLeftPanel = true;

    if (m_settings->contains(QLatin1String("fileManager/showLeftPanel")))
        showLeftPanel = m_settings->value(QLatin1String("fileManager/showLeftPanel")).toBool();

    QVariant value = m_settings->value(QLatin1String("fileManager/splitterState"));

    m_panel->setVisible(showLeftPanel);

    if (value.isValid()) {
        splitter->restoreState(value.toByteArray());
    } else {
        splitter->setSizes(QList<int>() << 200 << 600);
    }

    showLeftPanelAction->setChecked(showLeftPanel); // FIXME

    m_widget->blockSignals(true);
    int sortOrder = m_settings->value(QLatin1String("fileManager/sortingOrder")).toInt();
    int sortColumn = m_settings->value(QLatin1String("fileManager/sortingColumn")).toInt();
    int viewModeLeft = m_settings->value(QLatin1String("fileManager/viewModeLeft")).toInt();
    int viewModeRight = m_settings->value(QLatin1String("fileManager/viewModeRight")).toInt();
    bool dualPaneModeEnabled = m_settings->value(QLatin1String("fileManager/dualPaneModeEnabled")).toInt();
    int orientation = m_settings->value(QLatin1String("fileManager/orientation")).toInt();
    m_widget->setSortingOrder((Qt::SortOrder)sortOrder);
    m_widget->setSortingColumn((FileManagerWidget::Column)sortColumn);
    m_widget->rightWidget()->setViewMode((FileManagerWidget::ViewMode)viewModeRight);
    m_widget->setViewMode((FileManagerWidget::ViewMode)viewModeLeft);
    m_widget->setDualPaneModeEnabled(dualPaneModeEnabled);
    m_widget->setOrientation((Qt::Orientation)orientation);
    m_widget->blockSignals(false);
}

/*!
  \reimp
*/
bool FileManagerEditor::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    bool ok = true;
    bool visible;
    QByteArray splitterState, widgetState, baseState;

    s >> baseState;
    s >> visible;
    s >> splitterState;
    s >> widgetState;

    AbstractEditor::restoreState(baseState);
    m_panel->setVisible(visible);
    ok |= splitter->restoreState(splitterState);
    m_widget->blockSignals(true);
    ok |= m_widget->restoreState(widgetState);
    m_widget->blockSignals(false);

    return ok;
}

/*!
  \reimp
*/
QByteArray FileManagerEditor::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << AbstractEditor::saveState();
    s << !m_panel->isHidden();
    s << splitter->saveState();
    s << m_widget->saveState();

    return state;
}

/*!
  \reimp
*/
void FileManagerEditor::resizeEvent(QResizeEvent *e)
{
    splitter->resize(e->size());
}

/*!
  \internal

  Reemits DualPaneWidget::currentPathChanged() signal as an QUrl
*/
void FileManagerEditor::onCurrentPathChanged(const QString &path)
{
    emit urlChanged(QUrl::fromLocalFile(path));
    emit iconChanged(icon());
    emit titleChanged(title());
    emit windowTitleChanged(windowTitle());
}

/*!
  \internal

  Opens file in current tab
*/
void FileManagerEditor::onOpenRequested(const QString &path)
{
    emit openTriggered(QUrl::fromLocalFile(path));
}

void FileManagerEditor::onViewModeChanged(FileManagerWidget::ViewMode mode)
{
    int pane = m_widget->activePane();
    if (pane == DualPaneWidget::LeftPane)
        m_settings->setValue(QLatin1String("fileManager/viewModeLeft"), mode);
    else
        m_settings->setValue(QLatin1String("fileManager/viewModeRight"), mode);
}

/*!
  \internal
*/
void FileManagerEditor::onSortingChanged()
{
    int sortOrder = m_widget->sortingOrder();
    int sortColumn = m_widget->sortingColumn();

    m_settings->setValue(QLatin1String("fileManager/sortingOrder"), sortOrder);
    m_settings->setValue(QLatin1String("fileManager/sortingColumn"), sortColumn);
}

/*!
  \internal
*/
void FileManagerEditor::onOrientationChanged(Qt::Orientation orientation)
{
    m_settings->setValue(QLatin1String("fileManager/orientation"), orientation);
}

void FileManagerEditor::onDualPaneModeChanged(bool enabled)
{
    m_settings->setValue(QLatin1String("fileManager/dualPaneModeEnabled"), enabled);
}

/*!
  \internal

  Shows or hides left panel and sets default visibility.
*/
void FileManagerEditor::showLeftPanel(bool show)
{
    m_settings->setValue(QLatin1String("fileManager/showLeftPanel"), show);

    m_panel->setVisible(show);
}

/*!
 \internal

 Stores default splitter size.
*/
void FileManagerEditor::onSplitterMoved(int, int)
{
    m_settings->setValue("fileManager/splitterState", splitter->saveState());
}

/*!
 \internal
*/
void FileManagerEditor::onPathsDropped(const QString &destination, const QStringList &paths, Qt::DropAction action)
{
    FileSystemManager *fsManager = m_widget->leftWidget()->fileSystemManager();
    if (action == Qt::CopyAction)
        fsManager->copy(paths, destination);
    else if (action == Qt::MoveAction)
        fsManager->move(paths, destination);
    else if (action == Qt::LinkAction)
        fsManager->link(paths, destination);
}

/*!
 \internal
*/
void FileManagerEditor::openNewTab(const QStringList &paths)
{
    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    emit openNewEditorTriggered(urls);
}

/*!
 \internal
*/
void FileManagerEditor::openNewWindow(const QStringList &paths)
{
    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    emit openNewWindowTriggered(urls);
}

/*!
 \internal

 Creates UI objects.
*/
void FileManagerEditor::setupUi()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    NavigationModel *model = pm->object<NavigationModel>("navigationModel");
    connect(model, SIGNAL(pathsDropped(QString,QStringList,Qt::DropAction)),
            SLOT(onPathsDropped(QString,QStringList,Qt::DropAction)));

    splitter = new MiniSplitter(this);

    m_widget = new DualPaneWidget(splitter);
//    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_widget->setFocus();

    m_panel = new NavigationPanel(splitter);
    m_panel->setModel(model);

    splitter->addWidget(m_panel);
    splitter->addWidget(m_widget);
}

/*!
  \internal

  Connects UI objects to private slots.
*/
void FileManagerEditor::setupConnections()
{
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SLOT(onCurrentPathChanged(QString)));
    connect(m_widget, SIGNAL(openRequested(QString)), SLOT(onOpenRequested(QString)));
    connect(m_widget, SIGNAL(openNewTabRequested(QStringList)), SLOT(openNewTab(QStringList)));
    connect(m_widget, SIGNAL(openNewWindowRequested(QStringList)), SLOT(openNewWindow(QStringList)));
    connect(m_widget, SIGNAL(sortingChanged()), SLOT(onSortingChanged()));
    connect(m_widget, SIGNAL(viewModeChanged(FileManagerWidget::ViewMode)), SLOT(onViewModeChanged(FileManagerWidget::ViewMode)));
    connect(m_widget, SIGNAL(orientationChanged(Qt::Orientation)), SLOT(onOrientationChanged(Qt::Orientation)));
    connect(m_widget, SIGNAL(dualPaneModeChanged(bool)), SLOT(onDualPaneModeChanged(bool)));

    connect(m_panel, SIGNAL(triggered(QString)), m_widget, SLOT(setCurrentPath(QString)));

    connect(splitter, SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));
}

/*!
  \internal

  Creates all actions.
*/
void FileManagerEditor::createActions()
{
    showLeftPanelAction = new QAction(tr("Show left panel"), this);
    showLeftPanelAction->setCheckable(true);

    connect(showLeftPanelAction, SIGNAL(triggered(bool)), this, SLOT(showLeftPanel(bool)));
    addAction(showLeftPanelAction, Constants::Actions::ShowLeftPanel);

    registerAction(m_widget->action(DualPaneWidget::Open), Constants::Actions::Open);
//    registerAction(m_widget->action(DualPaneWidget::OpenInTab), Constants::Actions::Open);
//    registerAction(m_widget->action(DualPaneWidget::OpenInWindow), Constants::Actions::Open);
//    registerAction(m_widget->action(DualPaneWidget::SelectProgram), Constants::Actions::Open);
    registerAction(m_widget->action(DualPaneWidget::SyncPanes), Constants::Actions::SyncPanes);
    registerAction(m_widget->action(DualPaneWidget::SwapPanes), Constants::Actions::SwapPanes);
    registerAction(m_widget->action(DualPaneWidget::NewFolder), Constants::Actions::NewFolder);
    registerAction(m_widget->action(DualPaneWidget::CopyFiles), Constants::Actions::CopyFiles);
    registerAction(m_widget->action(DualPaneWidget::MoveFiles), Constants::Actions::MoveFiles);
    registerAction(m_widget->action(DualPaneWidget::Rename), Constants::Actions::Rename);
    registerAction(m_widget->action(DualPaneWidget::Remove), Constants::Actions::Remove);
    registerAction(m_widget->action(DualPaneWidget::ShowFileInfo), Constants::Actions::FileInfo);

    registerAction(m_widget->action(DualPaneWidget::Redo), Constants::Actions::Redo);
    registerAction(m_widget->action(DualPaneWidget::Undo), Constants::Actions::Undo);
    registerAction(m_widget->action(DualPaneWidget::Cut), Constants::Actions::Cut);
    registerAction(m_widget->action(DualPaneWidget::Copy), Constants::Actions::Copy);
    registerAction(m_widget->action(DualPaneWidget::Paste), Constants::Actions::Paste);
    registerAction(m_widget->action(DualPaneWidget::SelectAll), Constants::Actions::SelectAll);

    registerAction(m_widget->action(DualPaneWidget::ShowHiddenFiles), Constants::Actions::ShowHiddenFiles);

    registerAction(m_widget->action(DualPaneWidget::IconMode), Constants::Actions::IconMode);
    registerAction(m_widget->action(DualPaneWidget::ColumnMode), Constants::Actions::ColumnMode);
    registerAction(m_widget->action(DualPaneWidget::TreeMode), Constants::Actions::TreeMode);
    registerAction(m_widget->action(DualPaneWidget::CoverFlowMode), Constants::Actions::CoverFlowMode);
    registerAction(m_widget->action(DualPaneWidget::EnableDualPane), Constants::Actions::DualPane);
    registerAction(m_widget->action(DualPaneWidget::VerticalPanels), Constants::Actions::VerticalPanels);

    registerAction(m_widget->action(DualPaneWidget::SortByName), Constants::Actions::SortByName);
    registerAction(m_widget->action(DualPaneWidget::SortBySize), Constants::Actions::SortBySize);
    registerAction(m_widget->action(DualPaneWidget::SortByType), Constants::Actions::SortByType);
    registerAction(m_widget->action(DualPaneWidget::SortByDate), Constants::Actions::SortByDate);
    registerAction(m_widget->action(DualPaneWidget::SortDescendingOrder), Constants::Actions::SortByDescendingOrder);
}

FileManagerEditorFactory::FileManagerEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QStringList FileManagerEditorFactory::mimeTypes()
{
    return QStringList() << QLatin1String("inode/directory");
}

QByteArray FileManagerEditorFactory::id() const
{
    return "FileManager";
}

AbstractEditor * FileManagerEditorFactory::createEditor(QWidget *parent)
{
    return new FileManagerEditor(parent);
}

