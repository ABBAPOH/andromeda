#ifndef FILEMANAGERWIDGET_H
#define FILEMANAGERWIDGET_H

#include "filemanagerplugin_global.h"

#include <QtGui/QWidget>
#include <QtCore/QDir>
#include <history.h>

class QFileSystemModel;

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
public:
    enum ViewMode { ListView = 0, IconView, TableView, ColumnView, TreeView };
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
