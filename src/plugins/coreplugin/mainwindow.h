#ifndef MAINWINDOWPLUGIN_MAINWINDOW_H
#define MAINWINDOWPLUGIN_MAINWINDOW_H

#include "coreplugin_global.h"

#include <QtGui/QMainWindow>

class QSettings;
class QUrl;

namespace GuiSystem {
class AbstractEditor;
class StackedEditor;
}

namespace CorePlugin {

class MainWindowPrivate;
class COREPLUGIN_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int currentIndex() const;
    GuiSystem::StackedEditor *currentTab() const;
    int count() const;

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

    GuiSystem::AbstractEditor *currentEditor() const;

    static MainWindow *currentWindow();
    static QList<MainWindow*> windows();
    static MainWindow *createWindow();

public slots:
    void back();
    void forward();
    void up();

    void open(const QUrl &url);
    void openEditor(const QString &id);
    void openNewTab(const QUrl &url);
    void openNewTab(const QList<QUrl> &urls);
    static void openNewWindow(const QUrl &url);
    static void openNewWindow(const QList<QUrl> &urls);

    void newTab();
    static void newWindow();
    void closeTab(int index = -1);

    void refresh();
    void cancel();

    void nextTab();
    void prevTab();

protected:
    void resizeEvent(QResizeEvent *);

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace CorePlugin

#endif // MAINWINDOWPLUGIN_MAINWINDOW_H
