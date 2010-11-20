#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace Core {
    class EditorView;
}

class QLineEdit;
class QToolButton;
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

    Core::EditorView *view();
};

#endif // MAINWINDOW_H
