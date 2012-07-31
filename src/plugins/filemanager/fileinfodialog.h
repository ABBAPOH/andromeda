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

namespace FileManager {

class FileInfoDialogPrivate;
class FileInfoDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFileInfo fileInfo READ fileInfo WRITE setFileInfo NOTIFY fileInfoChanged)

public:
    explicit FileInfoDialog(QWidget *parent = 0);
    ~FileInfoDialog();

    QFileInfo fileInfo() const;
    void setFileInfo(const QFileInfo &info);

signals:
    void fileInfoChanged(const QFileInfo &info);

private slots:
    void onActivatedUser(int);
    void onActivatedGroup(int);
    void onActivatedOther(int);
    void updateSize();

private:
    FileInfoDialogPrivate *d;
};

} // namespace FileManager

#endif // FILEINFODIALOG_H
