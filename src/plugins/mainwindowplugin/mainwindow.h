#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace CorePlugin {
    class EditorView;
}

class QLineEdit;
class QToolButton;

namespace MainWindowPlugin {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newTab();
    void open(const QString &);
    void back();
    void forward();

private:
    Ui::MainWindow *ui;
    QLineEdit *m_lineEdit;
    QToolButton *m_newTabButton;

    CorePlugin::EditorView *view();
};

} // namespace MainWindowPlugin

#endif // MAINWINDOW_H
