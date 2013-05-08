#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include "filemanager_global.h"

#include <QWidget>

class QSplitter;
class QToolBar;

namespace FileManager {

class FileManagerWidget;
class NavigationModel;
class NavigationPanel;

class FileExplorerWidgetPrivate;
class FILEMANAGER_EXPORT FileExplorerWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileExplorerWidget)

    Q_PROPERTY(bool panelVisible READ isPanelVisible WRITE setPanelVisible NOTIFY panelVisibleChanged)
    Q_PROPERTY(bool statusBarVisible READ isStatusBarVisible WRITE setStatusBarVisible NOTIFY statusBarVisibleChanged)
public:
    explicit FileExplorerWidget(QWidget *parent = 0);
    explicit FileExplorerWidget(NavigationModel *model, QWidget *parent = 0);
    ~FileExplorerWidget();

    bool isPanelVisible() const;
    bool isStatusBarVisible() const;

    FileManagerWidget *widget() const;
    NavigationPanel *panel() const;
    QToolBar *statusBar() const;
    QSplitter *splitter() const;

    QAction *showLeftPanelAction() const;
    QAction *showStatusBarAction() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void setPanelVisible(bool visible);
    void setStatusBarVisible(bool visible);

signals:
    void panelVisibleChanged(bool visible);
    void statusBarVisibleChanged(bool visible);

protected slots:
    void onSelectedPathsChanged();

protected:
    FileExplorerWidgetPrivate *d_ptr;
};

} // namespace FileManager

#endif // FILEEXPLORERWIDGET_H
