#ifndef DUALPANEWIDGET_H
#define DUALPANEWIDGET_H

#include "filemanagerplugin_global.h"
#include "filemanagerwidget.h"

#include <QtGui/QWidget>
#include <coreplugin/history.h>

namespace FileManagerPlugin {

class DualPaneWidgetPrivate;
class FILEMANAGERPLUGIN_EXPORT DualPaneWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DualPaneWidget)
    Q_PROPERTY(Pane activePane READ activePane WRITE setActivePane NOTIFY activePaneChanged)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(FileManagerPlugin::FileManagerWidget::ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)

public:
    enum Pane { LeftPane = 0, RightPane = 1 };
    Q_ENUMS(Pane)

    explicit DualPaneWidget(QWidget *parent = 0);
    ~DualPaneWidget();

    CorePlugin::History *history() const;
    Pane activePane() const;
    FileManagerWidget *activeWidget() const;

    FileManagerWidget *leftWidget() const;
    FileManagerWidget *rightWidget() const;

    QString currentPath() const;

    bool dualPaneModeEnabled() const;
    FileManagerWidget::ViewMode viewMode() const;

    void setFileSystemManager(FileSystemManager *manager);

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
    FileManagerWidget *createPane();
    void createLeftPane();
    void createRightPane();
    void ensureRightPaneCreated();
    void updateState();

    bool eventFilter(QObject *, QEvent *);

protected:
    DualPaneWidgetPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // DUALPANEWIDGET_H
