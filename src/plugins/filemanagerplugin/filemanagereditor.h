#ifndef FILEMANAGEREDITOR_H
#define FILEMANAGEREDITOR_H

#include "abstracteditor.h"
#include "abstracteditorfactory.h"

class MiniSplitter;
class QActionGroup;
class QSignalMapper;

namespace CorePlugin {
class Settings;
}

namespace FileManagerPlugin {

class DualPaneWidget;
class NavigationPanel;

class FileManagerEditor : public CorePlugin::AbstractEditor
{
    Q_OBJECT
public:
    enum ViewMode {
        ListView = 0, IconView, ColumnView, TableView, TreeView, CoverFlow, DualPane
    };

    explicit FileManagerEditor(QWidget *parent = 0);

    // from AbstractEditor
    bool open(const QUrl &url);

    QUrl currentUrl() const;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentPathChanged(const QString &path);
    void onOpenRequested(const QString &path);
    void onCustomContextMenuRequested(const QPoint &pos);
    void setViewMode(int);
    void setAndSaveViewMode(int);
    void showLeftPanel(bool);
    void showFileInfo();
    void onSelectedPathsChanged();
    void onSplitterMoved(int,int);

private:
    void setupUi();
    void setupConnections();
    QAction * createAction(const QString &text, const QByteArray &id,
                           const char *slot, bool checkable = false);
    QAction * createViewAction(const QString &text, const QByteArray &id, int mode);
    void createActions();
    void createViewActions();
    void restoreDefaults();

private:
    MiniSplitter *splitter;
    DualPaneWidget *m_widget;
    NavigationPanel *m_panel;

    CorePlugin::Settings *m_settings;

    QAction *openAction;
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
};

class FileManagerEditorFactory : public CorePlugin::AbstractEditorFactory
{
public:
    explicit FileManagerEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    CorePlugin::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace FileManagerPlugin

#endif // FILEMANAGEREDITOR_H
