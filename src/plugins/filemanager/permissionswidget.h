#ifndef PERMISSIONSWIDGET_H
#define PERMISSIONSWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QFileInfo>

namespace Ui {
class PermissionsWidget;
}

class PermissionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PermissionsWidget(QWidget *parent = 0);
    ~PermissionsWidget();

    QFileInfo fileInfo() const;
    void setFileInfo(const QFileInfo &info);

private:
    void updateWidget();

private slots:
    void buttonPressed();
    void numericChanged();

private:
    Ui::PermissionsWidget *ui;

    QFileInfo m_fileInfo;
};

#endif // PERMISSIONSWIDGET_H
