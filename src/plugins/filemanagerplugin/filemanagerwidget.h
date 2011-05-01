#ifndef FILEMANAGERWIDGET_H
#define FILEMANAGERWIDGET_H

#include <QtGui/QWidget>

#include <history.h>

class QFileSystemModel;

namespace FileManagerPlugin {

class FileSystemUndoManager;

class FileManagerWidgetPrivate;
class FileManagerWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileManagerWidget)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
public:
    explicit FileManagerWidget(QWidget *parent = 0);
    ~FileManagerWidget();

    enum ViewMode { ListView = 0, IconView, TableView, ColumnView, TreeView };

    ViewMode viewMode() const;
    void setViewMode(ViewMode mode);

    QFileSystemModel *model() const;
    void setModel(QFileSystemModel *model);

    QString currentPath() const;

    CorePlugin::History *history() const;

    QStringList selectedPaths() const;

    FileSystemUndoManager *undoManager() const;

signals:
    void currentPathChanged(const QString &path);
    void canUndoChanged(bool);

public slots:
    void setCurrentPath(const QString &path);
    void copy();
    void paste();
    void remove();
    void undo();
    void redo();
    void back();
    void forward();

protected:
    FileManagerWidgetPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERWIDGET_H
