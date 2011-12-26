#include "filemanagereditor.h"
#include "filemanagereditor_p.h"

#include "filesystemmanager.h"
#include "filesystemmodel.h"
#include "fileinfodialog.h"
#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtCore/QUrl>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QFileIconProvider>

#include <extensionsystem/pluginmanager.h>
#include <widgets/minisplitter.h>

#include <coreplugin/constants.h>
#include <coreplugin/core.h>
#include <coreplugin/mainwindow.h>
#include <coreplugin/settings.h>

using namespace CorePlugin;
using namespace GuiSystem;
using namespace FileManagerPlugin;

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
    ignoreSignals(false)
{
    setupUi();
    setupConnections();
    createActions();
    setupSettings();

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
    m_widget->setCurrentPath(url.toLocalFile());
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

#include <QtCore/QBuffer>
/*!
  \reimp
*/
void FileManagerEditor::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QBuffer buffer(&state);
    buffer.open(QBuffer::ReadOnly);
    QDataStream s(&buffer);

    bool visible; QByteArray splitterState, widgetState, baseState;
    s >> baseState;
    s >> visible;
    s >> splitterState;
    s >> widgetState;

    AbstractEditor::restoreState(baseState);
    m_panel->setVisible(visible);
    splitter->restoreState(splitterState);
    m_widget->restoreState(widgetState);

    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());
}

/*!
  \reimp
*/
QByteArray FileManagerEditor::saveState() const
{
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    QDataStream s(&buffer);

    s << AbstractEditor::saveState();
    s << !m_panel->isHidden();
    s << splitter->saveState();
    s << m_widget->saveState();

    return buffer.data();
}

///*!
//  \reimp
//*/
//void FileManagerEditor::restoreSession(QSettings &s)
//{
//    AbstractEditor::restoreSession(s);
//    QVariant value;

//    value = s.value(QLatin1String("panelVisible"));
//    if (value.isValid())
//        m_panel->setVisible(value.toBool());

//    value = s.value(QLatin1String("splitterState"));
//    if (value.isValid()) {
//        splitter->restoreState(value.toByteArray());
//    }

//    m_widget->restoreState(s.value(QLatin1String("state")).toByteArray());

//    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());
//}

///*!
//  \reimp
//*/
//void FileManagerEditor::saveSession(QSettings &s)
//{
//    AbstractEditor::saveSession(s);

//    s.setValue(QLatin1String("panelVisible"), !m_panel->isHidden());
//    s.setValue(QLatin1String("splitterState"), splitter->saveState());
//    s.setValue(QLatin1String("state"), m_widget->saveState());
//}

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
//    mainWindow()->open(QUrl::fromLocalFile(path));
    emit openTriggered(QUrl::fromLocalFile(path));
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
        QMenu *openWithMenu = new QMenu(tr("Open with"), menu);
        openWithMenu->addSeparator();
        openWithMenu->addAction(selectProgramAction);

        menu->addAction(openAction);
        menu->addAction(openNewTabAction);
        menu->addAction(openNewWindowAction);
        menu->addMenu(openWithMenu);
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
*/
int FileManagerEditor::viewMode() const
{
    if (m_widget->dualPaneModeEnabled())
        return DualPane;
    return m_widget->viewMode();
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
    emit viewModeChanged(mode);
}

void FileManagerEditor::onViewModeChanged()
{
    FileManagerWidget::ViewMode viewMode = m_widget->leftWidget()->viewMode();

    ignoreSignals = true;
    iconModeAction->setChecked(viewMode == FileManagerWidget::IconView);
    columnModeAction->setChecked(viewMode == FileManagerWidget::ColumnView);
    treeModeAction->setChecked(viewMode == FileManagerWidget::TreeView);
    coverFlowModeAction->setChecked(viewMode == FileManagerWidget::CoverFlow);
    ignoreSignals = false;
}

/*!
  \internal

  Sets DualPaneWidget's view mode and stores is to settings as default mode.
*/
void FileManagerEditor::setAndSaveViewMode(int mode)
{
    if (ignoreSignals)
        return;

    m_settings->setValue(QLatin1String("fileManager/viewMode"), mode);

    m_widget->blockSignals(true);
    setViewMode(mode);
    m_widget->blockSignals(false);
}

/*!
  \internal
*/
void FileManagerEditor::setSortColumn(int column)
{
    if (ignoreSignals)
        return;

    m_widget->blockSignals(true);
    m_widget->setSortingColumn((FileManagerWidget::Column)column);
    m_widget->blockSignals(false);
}

/*!
  \internal
*/
void FileManagerEditor::setSortOrder(bool descending)
{
    if (ignoreSignals)
        return;

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

    ignoreSignals = true;
    sortByDescendingOrderAction->setChecked(sortOrder);

    sortByNameAction->setChecked(sortColumn == FileManagerWidget::NameColumn);
    sortBySizeAction->setChecked(sortColumn == FileManagerWidget::SizeColumn);
    sortByTypeAction->setChecked(sortColumn == FileManagerWidget::TypeColumn);
    sortByDateAction->setChecked(sortColumn == FileManagerWidget::DateColumn);
    ignoreSignals = false;
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
    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    emit openNewEditorTriggered(urls);
    //    QStringList paths = m_widget->activeWidget()->selectedPaths();
//    MainWindow *window = mainWindow();
//    foreach (const QString &path, paths) {
//        window->openNewTab(QUrl::fromLocalFile(path));
//    }
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

void FileManagerEditor::selectProgram()
{
    QString programsFolder;
    QVariant value = m_settings->value(QLatin1String("filemanager/programsFolder"));
    if (value.isValid()) {
        programsFolder = value.toString();
    } else {
#if defined(Q_OS_MAC)
        programsFolder = QLatin1String("/Applications");
#elif defined(Q_OS_WIN)
        programsFolder = QDriveInfo::rootDrive().rootPath() + QLatin1String("/Program Files");
#elif defined(Q_OS_UNIX)
        programsFolder = QLatin1String("/usr/bin");
#endif
    }

#ifdef Q_OS_WIN
    QString filter = tr("Programs (*.exe *.cmd *.com *.bat);;All files (*)");
#else
    QString filter;
#endif;

    QString programPath = QFileDialog::getOpenFileName(this, tr("Select program"), programsFolder, filter);
    if (programPath.isEmpty())
        return;

    m_settings->setValue(QLatin1String("filemanager/programsFolder"), QFileInfo(programPath).absolutePath());

    bool result = true;
    QStringList failedPaths;
    foreach (const QString path, m_widget->activeWidget()->selectedPaths()) {
        QString program;
        QStringList arguments;
#if defined(Q_OS_MAC)
        program = QLatin1String("open");
        arguments << QLatin1String("-a") << programPath << path;
#else
        program = programPath;
        arguments << path;
#endif
        bool r = QProcess::startDetached(program, arguments);
        if (!r)
            failedPaths.append(path);
        result &= r;
    }

    if (!result) {
        QMessageBox::warning(this,
                             tr("Can't open files"),
                             tr("Andromeda failed to open some files :%1").
                             arg(failedPaths.join(QLatin1String("\n"))));
    }
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
    connect(m_widget, SIGNAL(viewModeChanged(FileManagerWidget::ViewMode)), SLOT(onViewModeChanged()));
    connect(m_widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    connect(m_panel, SIGNAL(triggered(QString)), m_widget, SLOT(setCurrentPath(QString)));

    connect(splitter, SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));
}

/*!
  \internal
*/
void FileManagerEditor::setupSettings()
{
    m_settings = Core::instance()->settings();

    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/iconSize"));
    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/gridSize"));
    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/flow"));
}

/*!
  \internal

  Creates \a checkable action with \a text, connects it to DualPaneWidget's \a slot
  and registers it in ActionManager with \a id.
*/
QAction * FileManagerEditor::createAction(const QString &text, const QByteArray &id, const char *slot,
                                          bool checkable)
{
    QAction *action = new QAction(this);
    action->setText(text);
    action->setCheckable(checkable);
    if (!checkable)
        connect(action, SIGNAL(triggered()), m_widget, slot);
    else
        connect(action, SIGNAL(toggled(bool)), m_widget, slot);
    m_widget->addAction(action);
    registerAction(action, id);
    return action;
}

/*!
  \internal

  Creates action with \a text for view mode \a mode.
*/
QAction * FileManagerEditor::createViewAction(const QString &text, const QByteArray &id, int mode)
{
    QAction *action = new QAction(this);
    action->setText(text);
    action->setCheckable(true);
    viewModeGroup->addAction(action);

    viewModeMapper->setMapping(action, mode);
    connect(action, SIGNAL(toggled(bool)), viewModeMapper, SLOT(map()));

    m_widget->addAction(action);
    registerAction(action, id);

    return action;
}

/*!
  \internal
*/
QAction * FileManagerEditor::createSortByAction(const QString &text, const QByteArray &id, int mode)
{
    QAction *action = new QAction(this);
    action->setText(text);
    action->setCheckable(true);
    sortByGroup->addAction(action);

    sortByMapper->setMapping(action, mode);
    connect(action, SIGNAL(toggled(bool)), sortByMapper, SLOT(map()));

    m_widget->addAction(action);
    registerAction(action, id);

    return action;
}

/*!
  \internal

  Creates all actions.
*/
void FileManagerEditor::createActions()
{
    openAction = createAction(tr("Open"), Constants::Actions::Open, SLOT(open()));
    newFolderAction = createAction(tr("New Folder"), Constants::Actions::NewFolder, SLOT(newFolder()));
    renameAction = createAction(tr("Rename"), Constants::Actions::Rename, SLOT(rename()));
    removeAction = createAction(tr("Remove"), Constants::Actions::Remove, SLOT(remove()));

    openNewTabAction = new QAction(tr("Open in new tab"), this);
    connect(openNewTabAction, SIGNAL(triggered()), SLOT(openNewTab()));

    openNewWindowAction = new QAction(tr("Open in new window"), this);
    connect(openNewWindowAction, SIGNAL(triggered()), SLOT(openNewWindow()));

    selectProgramAction = new QAction(tr("Select program..."), this);
    connect(selectProgramAction, SIGNAL(triggered()), SLOT(selectProgram()));

    showFileInfoAction = new QAction(tr("File info"), this);
    connect(showFileInfoAction, SIGNAL(triggered()), this, SLOT(showFileInfo()));
    m_widget->addAction(showFileInfoAction);
    registerAction(showFileInfoAction, Constants::Actions::FileInfo);

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
//    showLeftPanelAction->setCheckable(true);

//    this->addAction(showLeftPanelAction);
    connect(showLeftPanelAction, SIGNAL(toggled(bool)), this, SLOT(showLeftPanel(bool)));
//    registerAction(showLeftPanelAction, Constants::Actions::ShowLeftPanel);
    addAction(showLeftPanelAction, Constants::Actions::ShowLeftPanel);

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
    viewModeGroup = new QActionGroup(this);
    viewModeMapper = new QSignalMapper(this);

    iconModeAction = createViewAction(tr("Icon view"), Constants::Actions::IconMode, IconView);
    columnModeAction = createViewAction(tr("Column view"), Constants::Actions::ColumnMode, ColumnView);
    treeModeAction = createViewAction(tr("Tree view"), Constants::Actions::TreeMode, TreeView);
    coverFlowModeAction = createViewAction(tr("Cover flow"), Constants::Actions::CoverFlowMode, CoverFlow);
    dualPaneModeAction = createViewAction(tr("Dual pane"), Constants::Actions::DualPane, DualPane);

    viewModeGroup->addAction(dualPaneModeAction);

    iconModeAction->setChecked(true);

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
    sortByDateAction = createSortByAction(tr("Sort by date"), Constants::Actions::SortByDate, FileManagerWidget::DateColumn);

    sortByNameAction->setChecked(true);

    sortByDescendingOrderAction = new QAction(tr("Descending order"), this);
    sortByDescendingOrderAction->setCheckable(true);
    connect(sortByDescendingOrderAction, SIGNAL(triggered(bool)), this, SLOT(setSortOrder(bool)));
    m_widget->addAction(sortByDescendingOrderAction);
    registerAction(sortByDescendingOrderAction, Constants::Actions::SortByDescendingOrder);

    connect(sortByMapper, SIGNAL(mapped(int)), SLOT(setSortColumn(int)));
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

    m_settings->addObject(this, QLatin1String("fileManager/viewMode"));

    m_panel->setVisible(showLeftPanel);

    if (value.isValid()) {
        splitter->restoreState(value.toByteArray());
    } else {
        splitter->setSizes(QList<int>() << 200 << 600);
    }

    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());
    showLeftPanelAction->setChecked(showLeftPanel); // FIXME

    int sortOrder = m_settings->value(QLatin1String("fileManager/sortingOrder")).toInt();
    int sortColumn = m_settings->value(QLatin1String("fileManager/sortingColumn")).toInt();
    m_widget->setSortingOrder((Qt::SortOrder)sortOrder);
    m_widget->setSortingColumn((FileManagerWidget::Column)sortColumn);
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

