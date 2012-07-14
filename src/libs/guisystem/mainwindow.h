#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guisystem_global.h"

#include <QtCore/QUrl>
#include <QtGui/QMainWindow>

class QToolButton;
class QUrl;

namespace GuiSystem {

class AbstractEditor;

class MainWindowPrivate;
class GUISYSTEM_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
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

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QAction *action(Action action) const;

    AbstractEditor *editor() const;
    virtual void setEditor(AbstractEditor *editor);

    bool menuVisible() const;

    QUrl url() const;

    QToolButton *menuBarButton() const;

    static MainWindow *currentWindow();
    static QList<MainWindow*> windows();
    static MainWindow *createWindow();

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
    void setWindowIcon(const QIcon &icon);

    virtual void onUrlChanged(const QUrl &url);

    virtual void startLoad();
    virtual void setLoadProgress(int progress);
    virtual void finishLoad(bool ok);

protected:
    MainWindowPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_H
