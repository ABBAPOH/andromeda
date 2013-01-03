#ifndef PERMISSIONSWIDGET_H
#define PERMISSIONSWIDGET_H

#include "filemanager_global.h"

#include <QtGui/QWidget>
#include <QtCore/QFileInfo>

namespace Ui {
class PermissionsWidget;
}

namespace FileManager {

class FILEMANAGER_EXPORT PermissionsWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFileInfo fileInfo READ fileInfo WRITE setFileInfo NOTIFY fileInfoChanged)

public:
    explicit PermissionsWidget(QWidget *parent = 0);
    ~PermissionsWidget();

    QFileInfo fileInfo() const;
    void setFileInfo(const QFileInfo &info);

signals:
    void fileInfoChanged(const QFileInfo &info);

private:
    void updateWidget();

private slots:
    void buttonPressed();
    void numericChanged();

private:
    Ui::PermissionsWidget *ui;

    QFileInfo m_fileInfo;
};

} // namespace FileManager

#endif // PERMISSIONSWIDGET_H
