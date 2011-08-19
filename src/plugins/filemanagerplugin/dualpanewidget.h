#ifndef DUALPANEWIDGET_H
#define DUALPANEWIDGET_H

#include "filemanagerplugin_global.h"

#include <QtGui/QWidget>
#include <history.h>

namespace FileManagerPlugin {

class FileManagerWidget;

class DualPaneWidgetPrivate;
class FILEMANAGERPLUGIN_EXPORT DualPaneWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DualPaneWidget)
    Q_PROPERTY(Pane activePane READ activePane WRITE setActivePane NOTIFY activePaneChanged)
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
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

signals:
    void activePaneChanged(Pane pane);
    void currentPathChanged(const QString &path);
    void openRequested(const QString &path);

public slots:
    void setActivePane(Pane pane);
    void setCurrentPath(const QString &path);
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
    void copy();
    void paste();
    void selectAll();

    void back();
    void forward();
    void up();
    void home();

protected:
    bool eventFilter(QObject * watched, QEvent * event);

    DualPaneWidgetPrivate *d_ptr;
};

} // namespace FileManagerPlugin

#endif // DUALPANEWIDGET_H
