#include "filemanagereditor.h"

#include "filesystemmanager.h"
#include "filesystemmodel.h"
#include "fileinfodialog.h"
#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtCore/QUrl>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QFileIconProvider>

#include <actionmanager.h>
#include <constants.h>
#include <core.h>
#include <mainwindow.h>
#include <minisplitter.h>
#include <pluginmanager.h>
#include <settings.h>

using namespace CorePlugin;
using namespace FileManagerPlugin;

FileManagerEditor::FileManagerEditor(QWidget *parent) :
    AbstractEditor(parent)
{
    setupUi();
    setupConnections();
    createActions();
    restoreDefaults();
}

/*!
  \reimp
*/
bool FileManagerEditor::open(const QUrl &url)
{
    m_widget->setCurrentPath(url.toLocalFile());
    return true;
}

/*!
  \reimp
*/
QUrl FileManagerEditor::currentUrl() const
{
    return QUrl::fromLocalFile(m_widget->currentPath());
}

/*!
  \reimp
*/
int FileManagerEditor::currentIndex() const
{
    if (m_widget->activePane() == DualPaneWidget::LeftPane)
        return m_widget->leftWidget()->history()->currentItemIndex(); // 0, 1, 2
    else
        return -m_widget->rightWidget()->history()->currentItemIndex() - 2; // -2, -3, -4
}

/*!
  \reimp
*/
void FileManagerEditor::setCurrentIndex(int index)
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

    QFileInfo fi(path);
    if (fi.exists())
        return fi.fileName();
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
  \reimp
*/

void FileManagerEditor::restoreSession(QSettings &s)
{
    AbstractEditor::restoreSession(s);
    QVariant value;

    int viewMode = s.value(QLatin1String("viewMode")).toInt();
    setViewMode(viewMode);

    iconModeAction->setChecked(viewMode == IconView);
    columnModeAction->setChecked(viewMode == ColumnView);
    treeModeAction->setChecked(viewMode == TreeView);
    coverFlowModeAction->setChecked(viewMode == CoverFlow);
    dualPaneModeAction->setChecked(viewMode == DualPane);

    value = s.value(QLatin1String("panelVisible"));
    if (value.isValid())
        m_panel->setVisible(value.toBool());

    value = s.value(QLatin1String("splitterState"));
    if (value.isValid()) {
        splitter->restoreState(value.toByteArray());
    }

    m_widget->setSortingOrder((Qt::SortOrder)s.value(QLatin1String("sortingOrder")).toInt());
    m_widget->setSortingColumn((FileManagerWidget::Column)s.value(QLatin1String("sortingColumn")).toInt());
}

/*!
  \reimp
*/
void FileManagerEditor::saveSession(QSettings &s)
{
    AbstractEditor::saveSession(s);

    int mode = 0;
    if (m_widget->dualPaneModeEnabled())
        mode = DualPane;
    else
        mode = m_widget->viewMode();

    s.setValue(QLatin1String("panelVisible"), !m_panel->isHidden());
    s.setValue(QLatin1String("viewMode"), mode);
    s.setValue(QLatin1String("splitterState"), splitter->saveState());

    s.setValue(QLatin1String("sortingOrder"), (int)m_widget->sortingOrder());
    s.setValue(QLatin1String("sortingColumn"), (int)m_widget->sortingColumn());
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
    emit currentUrlChanged(QUrl::fromLocalFile(path));
}

/*!
  \internal

  Opens file in current tab
*/
void FileManagerEditor::onOpenRequested(const QString &path)
{
    mainWindow()->open(QUrl::fromLocalFile(path));
}

/*!
  \internal

  Shows DualPaneWidget's context menu
*/
void FileManagerEditor::onCustomContextMenuRequested(const QPoint &pos)
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();

    QMenu *menu = new QMenu;
    if (paths.isEmpty()) {
        menu->addAction(newFolderAction);
        menu->addSeparator();
        menu->addAction(showFileInfoAction);
        menu->addSeparator();
        menu->addAction(pasteAction);
        menu->addAction(selectAllAction);
        menu->addSeparator();
        QMenu * viewModeMenu = menu->addMenu(tr("View Mode"));
        viewModeMenu->addAction(iconModeAction);
        viewModeMenu->addAction(columnModeAction);
        viewModeMenu->addAction(treeModeAction);
        viewModeMenu->addAction(coverFlowModeAction);
        viewModeMenu->addAction(dualPaneModeAction);
        QMenu * sortByMenu = menu->addMenu(tr("Sort by"));
        sortByMenu->addAction(sortByNameAction);
        sortByMenu->addAction(sortByTypeAction);
        sortByMenu->addAction(sortBySizeAction);
        sortByMenu->addAction(sortByDateAction);
        sortByMenu->addSeparator();
        sortByMenu->addAction(sortByDescendingOrderAction);
    } else {
        menu->addAction(openAction);
        menu->addAction(openNewTabAction);
        menu->addAction(openNewWindowAction);
        menu->addSeparator();
        menu->addAction(showFileInfoAction);
        menu->addSeparator();
        menu->addAction(renameAction);
        menu->addAction(removeAction);
        menu->addSeparator();
        menu->addAction(copyAction);
    }
    menu->exec(m_widget->mapToGlobal(pos));
    delete menu;
}

/*!
  \internal

  Sets DualPaneWidget's view mode.
*/
void FileManagerEditor::setViewMode(int mode)
{
    if (mode == DualPane) {
        m_widget->setDualPaneModeEnabled(true);
    } else {
        m_widget->setDualPaneModeEnabled(false);
        m_widget->setViewMode((FileManagerWidget::ViewMode)mode);
    }
}

/*!
  \internal

  Sets DualPaneWidget's view mode and stores is to settings as default mode.
*/
void FileManagerEditor::setAndSaveViewMode(int mode)
{
    m_settings->setValue(QLatin1String("fileManager/viewMode"), mode);

    setViewMode(mode);
}

/*!
  \internal
*/
void FileManagerEditor::setSortColumn(int column)
{
    m_widget->blockSignals(true);
    m_widget->setSortingColumn((FileManagerWidget::Column)column);
    m_widget->blockSignals(false);
}

/*!
  \internal
*/
void FileManagerEditor::setSortOrder(bool descending)
{
    m_widget->blockSignals(true);
    m_widget->setSortingOrder((Qt::SortOrder)descending);
    m_widget->blockSignals(false);
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

    sortByDescendingOrderAction->setChecked(sortOrder);

    sortByNameAction->setChecked(sortColumn == FileManagerWidget::NameColumn);
    sortBySizeAction->setChecked(sortColumn == FileManagerWidget::SizeColumn);
    sortByTypeAction->setChecked(sortColumn == FileManagerWidget::TypeColumn);
    sortByDateAction->setChecked(sortColumn == FileManagerWidget::DateChangedColumn);
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

  Shows FileInfoDialog.
*/
void FileManagerEditor::showFileInfo()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    if (paths.isEmpty())
        paths.append(m_widget->currentPath());

    foreach (const QString &path, paths) {
        FileInfoDialog *dialog = new FileInfoDialog(m_widget);
        dialog->setFileInfo(QFileInfo(path));
        dialog->show();
    }
}

/*!
  \internal

  Updates FileManager actions that depends on selection.
*/
void FileManagerEditor::onSelectedPathsChanged()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    bool enabled = !paths.empty();

    openAction->setEnabled(enabled);
    renameAction->setEnabled(enabled);
    removeAction->setEnabled(enabled);
//    cutAction->setEnabled(enabled);
    copyAction->setEnabled(enabled);

    if (!paths.isEmpty()) {
        if (paths.size() == 1) {
            cutAction->setText(tr("Cut \"%1\"").arg(QFileInfo(paths[0]).fileName()));
            copyAction->setText(tr("Copy \"%1\"").arg(QFileInfo(paths[0]).fileName()));
        } else {
            cutAction->setText(tr("Cut %1 items").arg(paths.size()));
            copyAction->setText(tr("Copy %1 items").arg(paths.size()));
        }
    } else {
        cutAction->setText(tr("Copy"));
        copyAction->setText(tr("Copy"));
    }
}

/*!
 \internal

 Stores default splitter size.
*/
void FileManagerEditor::onSplitterMoved(int, int)
{
    Core::instance()->settings()->setValue("fileManager/splitterState", splitter->saveState());
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
void FileManagerEditor::openNewTab()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    MainWindow *window = mainWindow();
    foreach (const QString &path, paths) {
        window->openNewTab(QUrl::fromLocalFile(path));
    }
}

/*!
 \internal
*/
void FileManagerEditor::openNewWindow()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    if (paths.isEmpty())
        return;

    MainWindow *window = MainWindow::createWindow();
    foreach (const QString &path, paths) {
        window->openNewTab(QUrl::fromLocalFile(path));
    }
    window->show();
}

/*!
 \internal

 Creates UI objects.
*/
void FileManagerEditor::setupUi()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    FileSystemManager *manager = pm->object<FileSystemManager>("fileSystemManager");
    NavigationModel *model = pm->object<NavigationModel>("navigationModel");
    connect(model, SIGNAL(pathsDropped(QString,QStringList,Qt::DropAction)),
            SLOT(onPathsDropped(QString,QStringList,Qt::DropAction)));

    splitter = new MiniSplitter(this);

    m_widget = new DualPaneWidget(splitter);
    m_widget->setFileSystemManager(manager);
    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
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
    connect(m_widget, SIGNAL(selectedPathsChanged()), SLOT(onSelectedPathsChanged()));
    connect(m_widget, SIGNAL(sortingChanged()), SLOT(onSortingChanged()));
    connect(m_widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    connect(m_panel, SIGNAL(triggered(QString)), m_widget, SLOT(setCurrentPath(QString)));

    connect(splitter, SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));
}

/*!
  \internal

  Creates \a checkable action with \a text, connects it to DualPaneWidget's \a slot
  and registers it in ActionManager with \a id.
*/
QAction * FileManagerEditor::createAction(const QString &text, const QByteArray &id, const char *slot,
                                          bool checkable)
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

     QAction *action = new QAction(this);
     action->setText(text);
     action->setCheckable(checkable);
     if (!checkable)
         connect(action, SIGNAL(triggered()), m_widget, slot);
     else
         connect(action, SIGNAL(toggled(bool)), m_widget, slot);
     m_widget->addAction(action);
     actionManager->registerAction(action, id);
     return action;
}

/*!
  \internal

  Creates action with \a text for view mode \a mode.
*/
QAction * FileManagerEditor::createViewAction(const QString &text, const QByteArray &id, int mode)
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    QAction *action = new QAction(this);
    action->setText(text);
    action->setCheckable(true);
    viewModeGroup->addAction(action);

    viewModeMapper->setMapping(action, mode);
    connect(action, SIGNAL(toggled(bool)), viewModeMapper, SLOT(map()));

    m_widget->addAction(action);
    actionManager->registerAction(action, id);

    return action;
}

/*!
  \internal
*/
QAction * FileManagerEditor::createSortByAction(const QString &text, const QByteArray &id, int mode)
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    QAction *action = new QAction(this);
    action->setText(text);
    action->setCheckable(true);
    sortByGroup->addAction(action);

    sortByMapper->setMapping(action, mode);
    connect(action, SIGNAL(toggled(bool)), sortByMapper, SLOT(map()));

    m_widget->addAction(action);
    actionManager->registerAction(action, id);

    return action;
}

/*!
  \internal

  Creates all actions.
*/
void FileManagerEditor::createActions()
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    openAction = createAction(tr("Open"), Constants::Actions::Open, SLOT(open()));
    newFolderAction = createAction(tr("New Folder"), Constants::Actions::NewFolder, SLOT(newFolder()));
    renameAction = createAction(tr("Rename"), Constants::Actions::Rename, SLOT(rename()));
    removeAction = createAction(tr("Remove"), Constants::Actions::Remove, SLOT(remove()));

    openNewTabAction = new QAction(tr("Open in new tab"), this);
    connect(openNewTabAction, SIGNAL(triggered()), SLOT(openNewTab()));

    openNewWindowAction = new QAction(tr("Open in new window"), this);
    connect(openNewWindowAction, SIGNAL(triggered()), SLOT(openNewWindow()));

    showFileInfoAction = new QAction(tr("File info"), this);
    connect(showFileInfoAction, SIGNAL(triggered()), this, SLOT(showFileInfo()));
    m_widget->addAction(showFileInfoAction);
    actionManager->registerAction(showFileInfoAction, Constants::Actions::FileInfo);

    redoAction = createAction(tr("Redo"), Constants::Actions::Redo, SLOT(redo()));
    undoAction = createAction(tr("Undo"), Constants::Actions::Undo, SLOT(undo()));
    redoAction->setEnabled(false);
    undoAction->setEnabled(false);
    connect(m_widget, SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)));
    connect(m_widget, SIGNAL(canUndoChanged(bool)), undoAction, SLOT(setEnabled(bool)));

    cutAction = createAction(tr("Cut"), Constants::Actions::Cut, SLOT(cut()));
    copyAction = createAction(tr("Copy"), Constants::Actions::Copy, SLOT(copy()));
    pasteAction = createAction(tr("Paste"), Constants::Actions::Paste, SLOT(paste()));
    selectAllAction = createAction(tr("Select all"), Constants::Actions::SelectAll, SLOT(selectAll()));

//    actionManager->command(Constants::Actions::Up)->action(m_widget, SLOT(up()));

    showHiddenFilesAction = createAction(tr("Show hidden files"), Constants::Actions::ShowHiddenFiles,
                                         SLOT(showHiddenFiles(bool)), true);

    showLeftPanelAction = new QAction(tr("Show left panel"), this);
    showLeftPanelAction->setCheckable(true);
    this->addAction(showLeftPanelAction);
    connect(showLeftPanelAction, SIGNAL(toggled(bool)), this, SLOT(showLeftPanel(bool)));
    actionManager->registerAction(showLeftPanelAction, Constants::Actions::ShowLeftPanel);

    openAction->setEnabled(false);
    renameAction->setEnabled(false);
    removeAction->setEnabled(false);
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);

    createViewActions();
    createSortByActions();
}

/*!
  \internal

  Creates view actions.
*/
void FileManagerEditor::createViewActions()
{
    int viewMode = m_widget->viewMode();

    viewModeGroup = new QActionGroup(this);
    viewModeMapper = new QSignalMapper(this);

    iconModeAction = createViewAction(tr("Icon view"), Constants::Actions::IconMode, IconView);
    columnModeAction = createViewAction(tr("Column view"), Constants::Actions::ColumnMode, ColumnView);
    treeModeAction = createViewAction(tr("Tree view"), Constants::Actions::TreeMode, TreeView);
    coverFlowModeAction = createViewAction(tr("Cover flow"), Constants::Actions::CoverFlowMode, CoverFlow);
    dualPaneModeAction = createViewAction(tr("Dual pane"), Constants::Actions::DualPane, DualPane);

    viewModeGroup->addAction(dualPaneModeAction);

    switch (viewMode) {
    case IconView: iconModeAction->setChecked(true); break;
    case ColumnView: columnModeAction->setChecked(true); break;
    case TreeView: treeModeAction->setChecked(true); break;
    case CoverFlow: coverFlowModeAction->setChecked(true); break;
    case DualPane: coverFlowModeAction->setChecked(true); break;
    default: break;
    }
    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());

    connect(viewModeMapper, SIGNAL(mapped(int)), SLOT(setAndSaveViewMode(int)));
}

void FileManagerPlugin::FileManagerEditor::createSortByActions()
{
    sortByGroup = new QActionGroup(this);
    sortByGroup->setExclusive(true);
    sortByMapper = new QSignalMapper(this);

    sortByNameAction = createSortByAction(tr("Sort by name"), Constants::Actions::SortByName, FileManagerWidget::NameColumn);
    sortBySizeAction = createSortByAction(tr("Sort by size"), Constants::Actions::SortBySize, FileManagerWidget::SizeColumn);
    sortByTypeAction = createSortByAction(tr("Sort by type"), Constants::Actions::SortByType, FileManagerWidget::TypeColumn);
    sortByDateAction = createSortByAction(tr("Sort by date"), Constants::Actions::SortByDate, FileManagerWidget::DateChangedColumn);

    sortByNameAction->setChecked(true);

    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();
    sortByDescendingOrderAction = new QAction(tr("Descending order"), this);
    sortByDescendingOrderAction->setCheckable(true);
    connect(sortByDescendingOrderAction, SIGNAL(triggered(bool)), this, SLOT(setSortOrder(bool)));
    m_widget->addAction(sortByDescendingOrderAction);
    actionManager->registerAction(sortByDescendingOrderAction, Constants::Actions::SortByDescendingOrder);

    connect(sortByMapper, SIGNAL(mapped(int)), SLOT(setSortColumn(int)));
}

/*!
  \internal

  Restores FileManagerEditor's default settings.
*/
void FileManagerEditor::restoreDefaults()
{
    m_settings = Core::instance()->settings();

    int mode = 1;
    bool showLeftPanel = true;
    QVariantList lst;

    if (m_settings->contains(QLatin1String("fileManager/viewMode")))
        mode = m_settings->value(QLatin1String("fileManager/viewMode")).toInt();

    if (m_settings->contains(QLatin1String("fileManager/showLeftPanel")))
        showLeftPanel = m_settings->value(QLatin1String("fileManager/showLeftPanel")).toBool();

    QVariant value = m_settings->value(QLatin1String("fileManager/splitterState"));

    setViewMode(mode);

    m_panel->setVisible(showLeftPanel);

    if (value.isValid()) {
        splitter->restoreState(value.toByteArray());
    } else {
        splitter->setSizes(QList<int>() << 200 << 600);
    }

    showLeftPanelAction->setChecked(showLeftPanel); // FIXME

    int sortOrder = m_settings->value(QLatin1String("fileManager/sortingOrder")).toInt();
    int sortColumn = m_settings->value(QLatin1String("fileManager/sortingColumn")).toInt();
    m_widget->setSortingOrder((Qt::SortOrder)sortOrder);
    m_widget->setSortingColumn((FileManagerWidget::Column)sortColumn);

    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/iconSize"));
    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/gridSize"));
    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/flow"));
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

