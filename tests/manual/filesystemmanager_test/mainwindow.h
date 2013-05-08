#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace FileManager {
class FileManagerWidget;
}

class QModelIndex;
class QFileSystemModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onClick(const QString & name);
    void onDoubleClick(const QModelIndex & index);

private:
    QFileSystemModel *model;
    FileManager::FileManagerWidget *widget;
};

#endif // MAINWINDOW_H
