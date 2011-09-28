#ifndef FILEMANAGEREDITOR_H
#define FILEMANAGEREDITOR_H

#include "abstracteditor.h"
#include "abstracteditorfactory.h"

class MiniSplitter;
class QActionGroup;

namespace FileManagerPlugin {

class DualPaneWidget;
class NavigationPanel;

class FileManagerEditor : public CorePlugin::AbstractEditor
{
    Q_OBJECT
public:
    explicit FileManagerEditor(QWidget *parent = 0);

    // from AbstractEditor
    bool open(const QString &path);

    QString currentPath() const;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onOpenRequested(const QString &path);
    void onCustomContextMenuRequested(const QPoint &pos);
    void setDualPaneModeEnabled(bool on);
    void setViewMode(int);
    void showFileInfo();
    void onSelectedPathsChanged();

private:
    QAction * createAction(const QString &text, const QByteArray &id,
                           QWidget *w, const char *slot,
                           bool checkable = false);
    void createActions();

private:
    MiniSplitter *splitter;
    DualPaneWidget *m_widget;
    NavigationPanel *m_panel;

    QAction *openAction;
    QAction *newFolderAction;
    QAction *renameAction;
    QAction *removeAction;
    QAction *showFileInfoAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;

    QAction *showHiddenFilesAction;

    QActionGroup *viewModeGroup;
    QAction *iconModeAction;
    QAction *columnModeAction;
    QAction *treeModeAction;
    QAction *dualPaneModeAction;
};

class FileManagerEditorFactory : public CorePlugin::AbstractEditorFactory
{
public:
    explicit FileManagerEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    CorePlugin::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace FileManagerPlugin

#endif // FILEMANAGEREDITOR_H
