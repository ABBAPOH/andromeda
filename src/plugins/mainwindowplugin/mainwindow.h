#ifndef MAINWINDOWPLUGIN_MAINWINDOW_H
#define MAINWINDOWPLUGIN_MAINWINDOW_H

#include <GuiSystem>

namespace MainWindowPlugin {

class MainWindowPrivate;
class MainWindow : public GuiSystem::MainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace MainWindowPlugin

#endif // MAINWINDOWPLUGIN_MAINWINDOW_H
