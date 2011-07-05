#ifndef MAINWINDOWPLUGIN_MAINWINDOW_H
#define MAINWINDOWPLUGIN_MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace MainWindowPlugin {

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
    void closeTab();

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace MainWindowPlugin

#endif // MAINWINDOWPLUGIN_MAINWINDOW_H
