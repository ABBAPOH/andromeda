#ifndef FILECOPYDIALOG_H
#define FILECOPYDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class FileCopyDialog;
    class FileCopyWidget;
}

class QFileCopier;

namespace FileManager {

class FileSystemManager;

class FileCopyDialogPrivate;
class FileCopyDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileCopyDialog)
public:
    explicit FileCopyDialog(QWidget *parent = 0);
    ~FileCopyDialog();

    FileSystemManager *fileSystemManager() const;
    void setFileSystemManager(FileSystemManager *manager);

protected:
    void resizeEvent(QResizeEvent *);

private:
    FileCopyDialogPrivate *d_ptr;
};

class FileCopyTask;
class FileCopyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileCopyWidget(QFileCopier *copier, QWidget *parent = 0);
    ~FileCopyWidget();

signals:
    void canceled();

public slots:
    void update();
    void updateProgress(qint64 progress);
//    void showDetails(bool show);

private:
    Ui::FileCopyWidget *ui;
    FileCopyTask *m_task;
};

} // namespace FileManager

#endif // FILECOPYDIALOG_H
