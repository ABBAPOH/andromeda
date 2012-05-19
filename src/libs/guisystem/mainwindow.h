#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guisystem_global.h"

#include <QtCore/QUrl>
#include <QtGui/QMainWindow>

namespace GuiSystem {

class AbstractContainer;

class MainWindowPrivate;
class GUISYSTEM_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)

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

                  NextEditor,
                  PreviousEditor,

                  ActionCount
                };

    typedef MainWindow * (*CreateWindowFunc)(void);

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QAction *action(Action action) const;

    AbstractContainer *contanier() const;
    virtual void setContanier(AbstractContainer *container);

    QUrl url() const;
    QList<QUrl> urls() const;

    static MainWindow *currentWindow();
    static QList<MainWindow*> windows();
    static MainWindow *createWindow();

    virtual bool restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

public slots:
    void back();
    void forward();

    virtual void open(const QUrl &url);

    void closeEditor();

    static void openNewWindow(const QUrl &url);
//    static void openNewWindow(const QList<QUrl> &urls);

    void save();
    void saveAs();

    void refresh();
    void cancel();

    void nextEditor();
    void previousEditor();

private slots:
    void setWindowIcon(const QIcon &icon);

protected:
    MainWindowPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // MAINWINDOW_H
