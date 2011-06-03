#ifndef FILECOPYDIALOG_H
#define FILECOPYDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class FileCopyReplaceDialog;
}

namespace FileManagerPlugin {

class FileCopyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileCopyDialog(QWidget *parent = 0);
    ~FileCopyDialog();

    void addWidget(QWidget *widget);

protected:
    void resizeEvent(QResizeEvent *);

private:
    Ui::FileCopyReplaceDialog *ui;
};

} // namespace FileManagerPlugin

#endif // FILECOPYDIALOG_H
