#ifndef FILECOPYREPLACEDIALOG_H
#define FILECOPYREPLACEDIALOG_H

#include <QDialog>

namespace Ui {
    class FileCopyReplaceDialog;
}

class FileCopyReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileCopyReplaceDialog(QWidget *parent = 0);
    ~FileCopyReplaceDialog();

signals:
    void cancelAll();

    void skip();
    void skipAll();

    void overwrite();
    void overwriteAll();

private slots:
    void onButtonClick();

private:
    Ui::FileCopyReplaceDialog *ui;
};

#endif // FILECOPYREPLACEDIALOG_H
