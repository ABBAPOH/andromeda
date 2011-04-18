#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace MainWindowPlugin

#endif // MAINWINDOW_H
