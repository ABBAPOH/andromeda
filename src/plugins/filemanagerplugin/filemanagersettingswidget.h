#ifndef FILEMANAGERSETTINGSWIDGET_H
#define FILEMANAGERSETTINGSWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class FileManagerSettingsWidget;
}

class FileManagerSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileManagerSettingsWidget(QWidget *parent = 0);
    ~FileManagerSettingsWidget();

private slots:
    void onChecked(bool);

private:
    Ui::FileManagerSettingsWidget *ui;
};

#endif // FILEMANAGERSETTINGSWIDGET_H
