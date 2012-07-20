#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <QWidget>

class QSplitter;

namespace FileManager {

class DualPaneWidget;
class NavigationModel;
class NavigationPanel;

class FileExplorerWidgetPrivate;
class FileExplorerWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileExplorerWidget)

    Q_PROPERTY(bool panelVisible READ isPanelVisible WRITE setPanelVisible NOTIFY panelVisibleChanged)
public:
    explicit FileExplorerWidget(QWidget *parent = 0);
    explicit FileExplorerWidget(NavigationModel *model, QWidget *parent = 0);
    ~FileExplorerWidget();

    bool isPanelVisible() const;

    DualPaneWidget *dualPane() const;
    NavigationPanel *panel() const;
    QSplitter *splitter() const;

    QAction *showLeftPanelAction() const;

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void setPanelVisible(bool visible);

signals:
    void panelVisibleChanged(bool visible);

protected:
    FileExplorerWidgetPrivate *d_ptr;
};

} // namespace FileManager

#endif // FILEEXPLORERWIDGET_H
