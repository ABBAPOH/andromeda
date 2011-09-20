#ifndef FILEMANAGERVIEW_H
#define FILEMANAGERVIEW_H

#include <QtCore/QPointer>
#include <ieditor.h>
#include <iviewfactory.h>

class QActionGroup;

namespace FileManagerPlugin {

class DualPaneWidget;
class FileManagerView : public CorePlugin::IEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(FileManagerView)

public:
    explicit FileManagerView(QObject *parent = 0);
    ~FileManagerView();

    // from IView
    void initialize();
    QString type() const;
    QWidget *widget() const;

    // from IEditor
    bool open(const QString &path);

    QString currentPath() const;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    void restoreSession(const QSettings &s);
    void saveSession(QSettings &s);

private slots:
    void onOpenRequested(const QString &path);
    void onCustomContextMenuRequested(const QPoint &pos);
    void setDualPaneModeEnabled(bool on);
    void setViewMode(int);

    void showFileInfo();

private:
    void createActions();

private:
    QPointer<DualPaneWidget> m_widget;
    const GuiSystem::State *m_state;

    QAction *openAction;
    QAction *newFolderAction;
    QAction *renameAction;
    QAction *removeAction;
    QAction *showFileInfoAction;

    QAction *undoAction;
    QAction *redoAction;
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

class FileManagerFactory : public GuiSystem::IViewFactory
{
    Q_OBJECT
public:
    explicit FileManagerFactory(QObject * parent = 0) : GuiSystem::IViewFactory(parent) {}
    ~FileManagerFactory() {}

    QString id() const;
    QString type() const;

protected:
    GuiSystem::IView *createView();
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERVIEW_H
