#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QToolBar;
class QTabWidget;
class QToolButton;
class QLineEdit;

namespace Core {
class EditorView;
}

namespace MainWindowPlugin {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:
    void newTab();
    void open(const QString &);

private:
    QToolBar *m_toolBar;
    QLineEdit *m_lineEdit;
    QTabWidget *m_tabWidget;
    QToolButton *m_newTabButton;

    Core::EditorView *view();
};

} // namespace MainWindowPlugin

#endif // MAINWINDOW_H
