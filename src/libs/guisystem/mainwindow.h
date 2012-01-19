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

    virtual void restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;

    static CreateWindowFunc createWindowFunc;

public slots:
    void back();
    void forward();

    void open(const QUrl &url);
    void openEditor(const QString &id);
    void openNewEditor(const QUrl &url);
    void openNewEditor(const QList<QUrl> &urls);

    void closeEditor();

    // TODO: Move to separate singletone aka Application?
    void openNewWindow(const QUrl &url);
    virtual void openNewWindow(const QList<QUrl> &urls);

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
