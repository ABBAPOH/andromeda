#ifndef MAINWINDOWPLUGIN_MAINWINDOW_H
#define MAINWINDOWPLUGIN_MAINWINDOW_H

#include "coreplugin_global.h"

#include <QtGui/QMainWindow>

class QSettings;
class QUrl;

namespace CorePlugin {

class AbstractEditor;
class Tab;

class MainWindowPrivate;
class COREPLUGIN_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int currentIndex() const;
    Tab *currentTab() const;
    int count() const;

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

    AbstractEditor *currentEditor() const;

    static MainWindow *currentWindow();
    static QList<MainWindow*> windows();

public slots:
    void back();
    void forward();
    void up();

    void open(const QUrl &url);
    void openNewTab(const QUrl &url);
    static void openNewWindow(const QUrl &url);

    void newTab();
    static void newWindow();
    void closeTab(int index = -1);

    void nextTab();
    void prevTab();

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace CorePlugin

#endif // MAINWINDOWPLUGIN_MAINWINDOW_H
