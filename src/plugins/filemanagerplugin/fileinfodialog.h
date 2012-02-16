#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QtCore/QFileInfo>
#include <QtGui/QWidget>

class DirectoryDetails;
class OutlineWidget;
class QFormLayout;
class QLabel;
class QVBoxLayout;
class QComboBox;

namespace FileManagerPlugin {

class FileInfoDialogPrivate;
class FileInfoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoDialog(QWidget *parent = 0);
    ~FileInfoDialog();

    QFileInfo fileInfo() const;
    void setFileInfo(const QFileInfo &info);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:

private slots:
    void onActivatedUser(int);
    void onActivatedGroup(int);
    void onActivatedOther(int);
    void updateSize();

private:
    FileInfoDialogPrivate *d;
};

} // namespace FileManagerPlugin

#endif // FILEINFODIALOG_H
