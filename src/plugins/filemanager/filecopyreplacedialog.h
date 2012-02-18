#ifndef FILECOPYREPLACEDIALOG_H
#define FILECOPYREPLACEDIALOG_H

#include <QDialog>

namespace Ui {
    class FileCopyReplaceDialog;
}

namespace FileManager {

class FileCopyReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileCopyReplaceDialog(QWidget *parent = 0);
    ~FileCopyReplaceDialog();

    void setIcon(const QIcon &icon);
    void setMessage(const QString &message);

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

} // namespace FileManager

#endif // FILECOPYREPLACEDIALOG_H
