#ifndef FILECOPYERRORDIALOG_H
#define FILECOPYERRORDIALOG_H

#include <QDialog>

namespace Ui {
    class FileCopyErrorDialog;
}

class QAbstractButton;
class FileCopyErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileCopyErrorDialog(QWidget *parent = 0);
    ~FileCopyErrorDialog();

    void setIcon(const QIcon &icon);
    void setMessage(const QString &message);

signals:
    void abort();

    void ignore();
    void retry();

private slots:
    void onButtonClick(QAbstractButton*);

private:
    Ui::FileCopyErrorDialog *ui;
};

#endif // FILECOPYERRORDIALOG_H
