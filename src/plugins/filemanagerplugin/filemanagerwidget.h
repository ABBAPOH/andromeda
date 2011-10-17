#ifndef FILEMANAGERWIDGET_H
#define FILEMANAGERWIDGET_H

#include "filemanagerplugin_global.h"

#include <QtGui/QWidget>
#include <QtCore/QDir>
#include <history.h>

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

    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QSize gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(Flow flow READ flow WRITE setFlow)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

public:
    enum ViewMode { ListView = 0, IconView, ColumnView, TableView, TreeView, CoverFlow, MaxViews };
    enum Flow { LeftToRight = 0, TopToBottom = 1 };
    Q_ENUMS(Flow)
    Q_ENUMS(ViewMode)

//    explicit FileManagerWidget(QWidget *parent = 0);
    explicit FileManagerWidget(FileSystemModel *model = 0, QWidget *parent = 0);
    ~FileManagerWidget();

    QString currentPath() const;

    FileSystemManager *fileSystemManager() const;

    CorePlugin::History *history() const;

    FileSystemModel *model() const;
    void setModel(FileSystemModel *model);

    QStringList selectedPaths() const;

    ViewMode viewMode() const;

    QSize gridSize() const;
    void setGridSize(QSize s);

    QSize iconSize() const;
    void setIconSize(QSize s);

    Flow flow() const;
    void setFlow(Flow flow);

signals:
    void currentPathChanged(const QString &path);
    void openRequested(const QString &path);
    void canUndoChanged(bool);
    void canRedoChanged(bool);
    void selectedPathsChanged();

public slots:
    void setCurrentPath(const QString &path);
    void setViewMode(ViewMode mode);

    void newFolder();
    void open();
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

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    FileManagerWidgetPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERWIDGET_H
