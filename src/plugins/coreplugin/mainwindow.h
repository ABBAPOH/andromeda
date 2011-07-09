#ifndef MAINWINDOWPLUGIN_MAINWINDOW_H
#define MAINWINDOWPLUGIN_MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace CorePlugin {

class MainWindowPrivate;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void back();
    void forward();

    void newTab();
    void closeTab(int index = -1);

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace CorePlugin

#endif // MAINWINDOWPLUGIN_MAINWINDOW_H
