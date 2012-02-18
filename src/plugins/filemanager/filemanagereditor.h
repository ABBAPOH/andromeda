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
class FileManagerHistory;

class FileManagerEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT

public:
    enum ViewMode {
        IconView = 0, ColumnView, TableView, TreeView, CoverFlow, DualPane
    };

    explicit FileManagerEditor(QWidget *parent = 0);

    Capabilities capabilities() const;

    // from AbstractEditor
    QUrl url() const;
    void open(const QUrl &url);

    GuiSystem::AbstractHistory *history() const;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    void restoreDefaults();
    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentPathChanged(const QString &path);
    void onOpenRequested(const QString &path);
    void onViewModeChanged(FileManagerWidget::ViewMode mode);
    void onSortingChanged();
    void showLeftPanel(bool);
    void onSplitterMoved(int,int);
    void onPathsDropped(const QString &, const QStringList &, Qt::DropAction);
    void openNewTab(const QStringList &paths);
    void openNewWindow(const QStringList &paths);

private:
    void setupUi();
    void setupConnections();
    void createActions();

private:
    MiniSplitter *splitter;
    DualPaneWidget *m_widget;
    NavigationPanel *m_panel;
    FileManagerHistory *m_history;

    QSettings *m_settings;

    QAction *showLeftPanelAction;

    bool ignoreSignals;
};

class FileManagerEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit FileManagerEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace FileManager

#endif // FILEMANAGEREDITOR_H
