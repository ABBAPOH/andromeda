#ifndef DUALPANEWIDGET_H
#define DUALPANEWIDGET_H

#include "filemanagerplugin_global.h"
#include "filemanagerwidget.h"

#include <QtGui/QWidget>
#include <guisystem/history.h>

namespace FileManagerPlugin {

class DualPaneWidgetPrivate;
class FILEMANAGERPLUGIN_EXPORT DualPaneWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DualPaneWidget)
    Q_PROPERTY(Pane activePane READ activePane WRITE setActivePane NOTIFY activePaneChanged)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(bool dualPaneModeEnabled READ dualPaneModeEnabled WRITE setDualPaneModeEnabled)
    Q_PROPERTY(FileManagerPlugin::FileManagerWidget::ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)

public:
    enum Pane { LeftPane = 0, RightPane = 1 };
    enum Action { NoAction = -1,
                  Open,
                  OpenInTab,
                  OpenInWindow,
                  SelectProgram,
                  NewFolder,
                  CopyFiles,
                  MoveFiles,
                  Rename,
                  Remove,
                  ShowFileInfo,

                  Redo,
                  Undo,
                  Cut,
                  Copy,
                  Paste,
                  SelectAll,
                  ShowHiddenFiles,

                  IconMode,
                  ColumnMode,
                  TreeMode,
                  CoverFlowMode,
                  EnableDualPane,

                  SortByName,
                  SortBySize,
                  SortByType,
                  SortByDate,
                  SortDescendingOrder,

                  ActionCount
                };
    Q_ENUMS(Pane)
    Q_ENUMS(Action)

    explicit DualPaneWidget(QWidget *parent = 0);
    ~DualPaneWidget();

    QAction *action(Action action) const;

    GuiSystem::History *history() const;
    Pane activePane() const;
    FileManagerWidget *activeWidget() const;

    FileManagerWidget *leftWidget() const;
    FileManagerWidget *rightWidget() const;

    QString currentPath() const;

    bool dualPaneModeEnabled() const;
    FileManagerWidget::ViewMode viewMode() const;

    QStringList selectedPaths() const;

    FileManagerWidget::Column sortingColumn() const;
    void setSortingColumn(FileManagerWidget::Column column);

    Qt::SortOrder sortingOrder() const;
    void setSortingOrder(Qt::SortOrder order);

    bool restoreState(const QByteArray &state);
    QByteArray saveState();

signals:
    void activePaneChanged(Pane pane);
    void currentPathChanged(const QString &path);

    void openRequested(const QString &path);
    void openNewTabRequested(const QStringList &paths);
    void openNewWindowRequested(const QStringList &paths);

    void canUndoChanged(bool);
    void canRedoChanged(bool);
    void selectedPathsChanged();
    void sortingChanged();
    void viewModeChanged(FileManagerWidget::ViewMode mode);

public slots:
    void setActivePane(Pane pane);
    void setCurrentPath(const QString &path);
    void setViewMode(FileManagerWidget::ViewMode mode);
    void setDualPaneModeEnabled(bool on);
    void sync();

    void newFolder();
    void open();
    void selectProgram();
    void showFileInfo();
    void remove();
    void rename();
    void copyFiles();
    void moveFiles();

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();

    void back();
    void forward();
    void up();

    void showHiddenFiles(bool show);

protected:
    bool eventFilter(QObject *, QEvent *);

protected:
    DualPaneWidgetPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // DUALPANEWIDGET_H
