#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guisystem_global.h"

#include <QtCore/QUrl>
#include <QtGui/QMainWindow>

class QToolButton;
class QUrl;

namespace GuiSystem {

class AbstractEditor;

class EditorWindowPrivate;
class GUISYSTEM_EXPORT EditorWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorWindow)
    Q_PROPERTY(bool menuVisible READ menuVisible WRITE setMenuVisible NOTIFY menuVisibleChanged)

public:
    enum Action { NoAction = -1,
                  OpenFile,
                  Close,
                  Save,
                  SaveAs,
                  Refresh,
                  Cancel,
                  Back,
                  Forward,

                  ShowMenu,

                  ActionCount
                };

    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow();

    QAction *action(Action action) const;

    AbstractEditor *editor() const;
    virtual void setEditor(AbstractEditor *editor);

    bool menuVisible() const;

    QUrl url() const;

    QToolButton *menuBarButton() const;

    static EditorWindow *currentWindow();
    static QList<EditorWindow*> windows();
    static EditorWindow *createWindow();

    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

public slots:
    void back();
    void forward();

    virtual void open(const QUrl &url);

    virtual void close();

    static void openNewWindow(const QUrl &url);
//    static void openNewWindow(const QList<QUrl> &urls);

    void save();
    void saveAs();

    void refresh();
    void cancel();

    void setMenuVisible(bool visible);

signals:
    void menuVisibleChanged(bool visible);

protected slots:
    virtual void onUrlChanged(const QUrl &url);

    virtual void onWindowIconChanged(const QIcon &icon);
    virtual void onWindowTitleChanged(const QString &title);

    virtual void startLoad();
    virtual void setLoadProgress(int progress);
    virtual void finishLoad(bool ok);

    virtual void onModificationChanged(bool modified);
    virtual void onReadOnlyChanged(bool readOnly);

protected:
    EditorWindowPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_H
