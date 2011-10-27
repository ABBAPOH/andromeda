#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QtCore/QFileInfo>
#include <QtGui/QWidget>
#include <QDriveInfo>

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

private slots:
    void onActivatedUser(int);
    void onActivatedGroup(int);
    void onActivatedOther(int);

private:
    Ui::FileInfoDialog *ui;
    QFileInfo m_fileInfo;
    QDriveInfo m_driveInfo;
};

#endif // FILEINFODIALOG_H
