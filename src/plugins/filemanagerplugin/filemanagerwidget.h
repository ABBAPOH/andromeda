#ifndef FILEMANAGERWIDGET_H
#define FILEMANAGERWIDGET_H

#include "filemanagerplugin_global.h"

#include <QtCore/QDir>
#include <QtGui/QWidget>
#include <guisystem/history.h>

class QFileSystemModel;
class QAbstractItemView;

namespace FileManagerPlugin {

class FileSystemUndoManager;
class FileSystemManager;
class FileSystemModel;

class FileManagerWidgetPrivate;
class FILEMANAGERPLUGIN_EXPORT FileManagerWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileManagerWidget)
    Q_DISABLE_COPY(FileManagerWidget)

    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(Flow flow READ flow WRITE setFlow)
    Q_PROPERTY(QSize gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(QStringList selectedPaths READ selectedPaths NOTIFY currentPathChanged)
    Q_PROPERTY(Column sortingColumn READ sortingColumn WRITE setSortingColumn NOTIFY sortingChanged)
    Q_PROPERTY(Qt::SortOrder sortingOrder READ sortingOrder WRITE setSortingOrder NOTIFY sortingChanged)
    Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)

public:
    enum ViewMode { IconView = 0, ColumnView, TreeView, CoverFlow, MaxViews };
    enum Flow { LeftToRight = 0, TopToBottom = 1 };
    enum Column { NameColumn = 0, SizeColumn, TypeColumn, DateColumn, ColumnCount };
    enum Action { NoAction = -1,
                  Open,
                  OpenInTab,
                  OpenInWindow,
                  SelectProgram,
                  NewFolder,
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

                  SortByName,
                  SortBySize,
                  SortByType,
                  SortByDate,
                  SortDescendingOrder,

                  ActionCount
                };

    Q_ENUMS(Flow)
    Q_ENUMS(ViewMode)
    Q_ENUMS(Column)
    Q_ENUMS(Action)

    explicit FileManagerWidget(QWidget *parent = 0);
    ~FileManagerWidget();

    QAction *action(Action action) const;

    bool alternatingRowColors() const;
    void setAlternatingRowColors(bool);

    bool canRedo() const;
    bool canUndo() const;

    QString currentPath() const;

    Flow flow() const;
    void setFlow(Flow flow);

    QSize gridSize() const;
    void setGridSize(QSize s);

    QSize iconSize(FileManagerWidget::ViewMode mode) const;
    void setIconSize(FileManagerWidget::ViewMode mode, QSize size);

    bool itemsExpandable() const;
    void setItemsExpandable(bool expandable);

    QStringList selectedPaths() const;

    Column sortingColumn() const;
    void setSortingColumn(Column column);

    Qt::SortOrder sortingOrder() const;
    void setSortingOrder(Qt::SortOrder order);

    void setSorting(Column column, Qt::SortOrder order);

    ViewMode viewMode() const;
    void setViewMode(ViewMode mode);

    FileSystemManager *fileSystemManager() const;

    GuiSystem::History *history() const;

    FileSystemModel *model() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState();

signals:
    void canRedoChanged(bool);
    void canUndoChanged(bool);
    void currentPathChanged(const QString &path);
    void selectedPathsChanged();
    void sortingChanged();
    void viewModeChanged(FileManagerWidget::ViewMode mode);

    void openRequested(const QString &path);
    void openNewTabRequested(const QStringList &paths);
    void openNewWindowRequested(const QStringList &paths);

public slots:
    void setCurrentPath(const QString &path);

    void newFolder();
    void open();
    void selectProgram();
    void showFileInfo();
    void remove();
    void rename();

    void undo();
    void redo();
    void copy();
    void paste();
    void selectAll();

    void back();
    void forward();
    void up();

    void showHiddenFiles(bool show);
    void showContextMenu(QPoint pos);

protected:
    void contextMenuEvent(QContextMenuEvent *);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    FileManagerWidgetPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERWIDGET_H
