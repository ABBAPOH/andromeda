#ifndef FILECOPYDIALOG_H
#define FILECOPYDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class FileCopyDialog;
    class FileCopyWidget;
}

namespace FileManagerPlugin {

class FileCopyDialogPrivate;
class FileCopyDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileCopyDialog)
public:
    explicit FileCopyDialog(QWidget *parent = 0);
    ~FileCopyDialog();

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
    explicit FileCopyWidget(FileCopyTask *task, QWidget *parent = 0);
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

} // namespace FileManagerPlugin

#endif // FILECOPYDIALOG_H
