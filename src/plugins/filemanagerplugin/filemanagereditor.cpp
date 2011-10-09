#include "filemanagereditor.h"

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

static QVariantList intListToVariantList(const QList<int> list)
{
    QVariantList lst;
    foreach (int i, list) {
        lst.append(i);
    }
    return lst;
}

static QList<int> variantListToIntList(const QVariantList &list)
{
    QList<int> result;
    foreach (const QVariant &v, list) {
        result.append(v.toInt());
    }
    return result;
}

using namespace CorePlugin;
using namespace FileManagerPlugin;

FileManagerEditor::FileManagerEditor(QWidget *parent) :
    AbstractEditor(parent)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    bool enableDualPane = settings.value("dualPaneModeEnabled").toBool();
    int mode = settings.value("viewMode").toInt();
    mode = mode == 0 ? 1 : mode;
    mode = enableDualPane ? -1 : mode;
    bool showLeftPanel = settings.contains("showLeftPanel") ? settings.value("showLeftPanel").toBool() : true;
    settings.endGroup();

    FileSystemModel *model = ExtensionSystem::PluginManager::instance()->object<FileSystemModel>("FileSystemModel");

    splitter = new MiniSplitter(this);

    m_widget = new DualPaneWidget(model,  splitter);
    if (!enableDualPane)
        setViewMode(mode);
    m_widget->setDualPaneModeEnabled(enableDualPane);
    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_widget->setFocus();
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SLOT(onCurrentPathChanged(QString)));
    connect(m_widget, SIGNAL(openRequested(QString)), SLOT(onOpenRequested(QString)));
    connect(m_widget, SIGNAL(selectedPathsChanged()), SLOT(onSelectedPathsChanged()));
    connect(m_widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    m_panel = new NavigationPanel(splitter);
    m_panel->setModel(ExtensionSystem::PluginManager::instance()->object<NavigationModel>("navigationModel"));
    connect(m_panel, SIGNAL(triggered(QString)), m_widget, SLOT(setCurrentPath(QString)));
    m_panel->setVisible(showLeftPanel);

    splitter->addWidget(m_panel);
    splitter->addWidget(m_widget);

    QVariantList lst = settings.value(QLatin1String("FileManager/lastSplitterSizes")).toList();
    QList<int> sizes;
    if (!lst.isEmpty()) {
        sizes = variantListToIntList(lst);
    } else {
        sizes << 200 << 600;
    }
    splitter->setSizes(sizes);

    connect(splitter, SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));

    createActions();
    showLeftPanelAction->setChecked(showLeftPanel); // FIXME
}

bool FileManagerEditor::open(const QUrl &url)
{
    m_widget->setCurrentPath(url.toLocalFile());
    return true;
}

QUrl FileManagerEditor::currentUrl() const
{
    return QUrl::fromLocalFile(m_widget->currentPath());
}

int FileManagerEditor::currentIndex() const
{
    if (m_widget->activePane() == DualPaneWidget::LeftPane)
        return m_widget->leftWidget()->history()->currentItemIndex(); // 0, 1, 2
    else
        return -m_widget->rightWidget()->history()->currentItemIndex() - 2; // -2, -3, -4
}

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

QIcon FileManagerEditor::icon() const
{
    return QFileIconProvider().icon(QFileInfo(m_widget->currentPath()));
}

QString FileManagerPlugin::FileManagerEditor::title() const
{
    QString path = m_widget->currentPath();
    if (path.endsWith(QLatin1Char('/')))
        path = path.left(path.length() - 1);

    QFileInfo fi(path);
    if (fi.exists())
        return fi.fileName();
    return QString();
}

QString FileManagerEditor::windowTitle() const
{
    return title();
}

void FileManagerEditor::restoreSession(QSettings &s)
{
    AbstractEditor::restoreSession(s);

    int viewMode = s.value(QLatin1String("viewMode")).toInt();
    bool dualPaneModeEnabled = s.value(QLatin1String("dualPaneModeEnabled")).toBool();
    m_widget->setViewMode((FileManagerWidget::ViewMode)viewMode);
    m_widget->setDualPaneModeEnabled(dualPaneModeEnabled);

    iconModeAction->setChecked(viewMode == FileManagerWidget::IconView);
    columnModeAction->setChecked(viewMode == FileManagerWidget::ColumnView);
    treeModeAction->setChecked(viewMode == FileManagerWidget::TreeView);
    coverFlowModeAction->setChecked(viewMode == FileManagerWidget::CoverFlow);
    dualPaneModeAction->setChecked(dualPaneModeEnabled);

    QVariantList lst = s.value(QLatin1String("splitterSizes")).toList();
    QList<int> sizes;
    foreach (const QVariant &v, lst) {
        sizes.append(v.toInt());
    }
    splitter->setSizes(sizes);
}

void FileManagerEditor::saveSession(QSettings &s)
{
    AbstractEditor::saveSession(s);
    s.setValue(QLatin1String("viewMode"), (int)m_widget->viewMode());
    s.setValue(QLatin1String("dualPaneModeEnabled"), m_widget->dualPaneModeEnabled());

    QVariantList lst;
    foreach (int size, splitter->sizes()) {
        lst.append(size);
    }
    s.setValue(QLatin1String("splitterSizes"), lst);
}

void FileManagerEditor::resizeEvent(QResizeEvent *e)
{
    splitter->resize(e->size());
}

void FileManagerEditor::onCurrentPathChanged(const QString &path)
{
    emit currentUrlChanged(QUrl::fromLocalFile(path));
}

void FileManagerEditor::onOpenRequested(const QString &path)
{
    mainWindow()->open(QUrl::fromLocalFile(path));
}

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
    } else {
        menu->addAction(openAction);
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

void FileManagerEditor::setDualPaneModeEnabled(bool on)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("dualPaneModeEnabled", on);
    settings.endGroup();
    m_widget->setDualPaneModeEnabled(on);
}

void FileManagerEditor::setViewMode(int mode)
{
    setDualPaneModeEnabled(false);

    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("viewMode", mode);
    settings.endGroup();
    m_widget->setViewMode((FileManagerWidget::ViewMode)mode);
}

void FileManagerEditor::showLeftPanel(bool show)
{
    QSettings s;
    s.beginGroup("FileManager");
    s.setValue("showLeftPanel", show);
    s.endGroup();

    m_panel->setVisible(show);
}

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

void FileManagerEditor::onSplitterMoved(int, int)
{
    QVariant list = intListToVariantList(splitter->sizes());
    Core::instance()->settings()->setValue("FileManager/lastSplitterSizes", list);
}

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

void FileManagerEditor::createActions()
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    openAction = createAction(tr("Open"), Constants::Actions::Open, SLOT(open()));
    newFolderAction = createAction(tr("New Folder"), Constants::Actions::NewFolder, SLOT(newFolder()));
    renameAction = createAction(tr("Rename"), Constants::Actions::Rename, SLOT(rename()));
    removeAction = createAction(tr("Remove"), Constants::Actions::Remove, SLOT(remove()));

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
}

void FileManagerEditor::createViewActions()
{
    int viewMode = m_widget->viewMode();

    viewModeGroup = new QActionGroup(this);
    viewModeMapper = new QSignalMapper(this);

    iconModeAction = createViewAction(tr("Icon view"), Constants::Actions::IconMode, FileManagerWidget::IconView);
    columnModeAction = createViewAction(tr("Column view"), Constants::Actions::ColumnMode, FileManagerWidget::ColumnView);
    treeModeAction = createViewAction(tr("Tree view"), Constants::Actions::TreeMode, FileManagerWidget::TableView);
    coverFlowModeAction = createViewAction(tr("Cover flow"), Constants::Actions::CoverFlowMode, FileManagerWidget::CoverFlow);
    dualPaneModeAction = createAction(tr("Dual pane"), Constants::Actions::DualPane,
                                      SLOT(setDualPaneModeEnabled(bool)), true);

    viewModeGroup->addAction(dualPaneModeAction);

    switch (viewMode) {
    case FileManagerWidget::IconView: iconModeAction->setChecked(true); break;
    case FileManagerWidget::ColumnView: columnModeAction->setChecked(true); break;
    case FileManagerWidget::TableView: treeModeAction->setChecked(true); break;
    case FileManagerWidget::CoverFlow: coverFlowModeAction->setChecked(true); break;
    default: break;
    }
    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());

    connect(viewModeMapper, SIGNAL(mapped(int)), SLOT(setViewMode(int)));
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

