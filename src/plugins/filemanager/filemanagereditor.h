#ifndef FILEMANAGEREDITOR_H
#define FILEMANAGEREDITOR_H

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

#include "filemanagerwidget.h"

class MiniSplitter;
class QSettings;

namespace FileManager {

class FileManagerWidget;
class DualPaneWidget;
class NavigationPanel;
class FileManagerEditorHistory;
class FileExplorerWidget;

class FileManagerEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT

public:
    enum ViewMode {
        IconView = 0, ColumnView, TableView, TreeView, DualPane
    };

    explicit FileManagerEditor(QWidget *parent = 0);

    // from AbstractEditor
    QUrl url() const;
    void open(const QUrl &url);

    GuiSystem::IHistory *history() const;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    void restoreDefaults();
    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

    void clear();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentPathChanged(const QString &path);
    void onSelectedPathsChanged();
    void onViewModeChanged(FileManagerWidget::ViewMode mode);
    void onSortingChanged();
    void onOrientationChanged(Qt::Orientation orientation);
    void onDualPaneModeChanged(bool);
    void initRightPane(bool);
    void onPanelVisibleChanged(bool);
    void onStatusBarVisibleChanged(bool);
    void onSplitterMoved(int,int);
    void openNewTab(const QStringList &paths);
    void openNewWindow(const QStringList &paths);
    void openEditor();
    void showContextMenu(const QPoint &pos);

private:
    void setupUi();
    void setupConnections();
    void createActions();
    void retranslateUi();
    void registerWidgetActions(FileManagerWidget *widget);

private:
    FileExplorerWidget *m_widget;
    FileManagerEditorHistory *m_history;

    QSettings *m_settings;
    QAction *m_openEditorAction;

    bool ignoreSignals;
};

class FileManagerEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit FileManagerEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;
    QStringList mimeTypes() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace FileManager

#endif // FILEMANAGEREDITOR_H
