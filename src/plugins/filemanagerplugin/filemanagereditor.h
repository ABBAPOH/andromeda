#ifndef FILEMANAGEREDITOR_H
#define FILEMANAGEREDITOR_H

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

class MiniSplitter;
class QActionGroup;
class QSignalMapper;

namespace CorePlugin {
class Settings;
}

namespace FileManagerPlugin {

class DualPaneWidget;
class NavigationPanel;
class FileManagerHistory;

class FileManagerEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
public:
    enum ViewMode {
        IconView = 0, ColumnView, TableView, TreeView, CoverFlow, DualPane
    };

    explicit FileManagerEditor(QWidget *parent = 0);

    Capabilities capabilities() const;

    // from AbstractEditor
    QUrl url() const;
    void open(const QUrl &url);

    GuiSystem::AbstractHistory *history() const;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    void restoreDefaults();
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;
//    void restoreSession(QSettings &s);
//    void saveSession(QSettings &s);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentPathChanged(const QString &path);
    void onOpenRequested(const QString &path);
    void onCustomContextMenuRequested(const QPoint &pos);
    void setViewMode(int);
    void onViewModeChanged();
    void setAndSaveViewMode(int);
    void setSortColumn(int);
    void setSortOrder(bool);
    void onSortingChanged();
    void showLeftPanel(bool);
    void showFileInfo();
    void onSelectedPathsChanged();
    void onSplitterMoved(int,int);
    void onPathsDropped(const QString &, const QStringList &, Qt::DropAction);
    void openNewTab();
    void openNewWindow();
    void selectProgram();

private:
    void setupUi();
    void setupConnections();
    void setupSettings();
    QAction * createAction(const QString &text, const QByteArray &id,
                           const char *slot, bool checkable = false);
    QAction * createViewAction(const QString &text, const QByteArray &id, int mode);
    QAction * createSortByAction(const QString &text, const QByteArray &id, int mode);
    void createActions();
    void createViewActions();
    void createSortByActions();

private:
    MiniSplitter *splitter;
    DualPaneWidget *m_widget;
    NavigationPanel *m_panel;
    FileManagerHistory *m_history;

    CorePlugin::Settings *m_settings;

    QAction *openAction;
    QAction *openNewTabAction;
    QAction *openNewWindowAction;
    QAction *selectProgramAction;
    QAction *newFolderAction;
    QAction *renameAction;
    QAction *removeAction;
    QAction *showFileInfoAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;

    QAction *showHiddenFilesAction;
    QAction *showLeftPanelAction;

    QSignalMapper *viewModeMapper;

    QActionGroup *viewModeGroup;
    QAction *iconModeAction;
    QAction *columnModeAction;
    QAction *treeModeAction;
    QAction *coverFlowModeAction;
    QAction *dualPaneModeAction;

    QSignalMapper *sortByMapper;

    QActionGroup *sortByGroup;
    QAction *sortByNameAction;
    QAction *sortBySizeAction;
    QAction *sortByTypeAction;
    QAction *sortByDateAction;

    QAction *sortByDescendingOrderAction;
    bool ignoreSignals;
};

class FileManagerEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit FileManagerEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace FileManagerPlugin

#endif // FILEMANAGEREDITOR_H
