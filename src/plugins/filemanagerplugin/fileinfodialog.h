#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QtCore/QFileInfo>
#include <QtGui/QWidget>

namespace Ui {
    class FileInfoDialog;
}

class FileInfoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoDialog(QWidget *parent = 0);
    ~FileInfoDialog();

    QFileInfo fileInfo() const;
    void setFileInfo(const QFileInfo &info);

private:
    void updateUi();

private:
    Ui::FileInfoDialog *ui;
    QFileInfo m_fileInfo;
};

#endif // FILEINFODIALOG_H
