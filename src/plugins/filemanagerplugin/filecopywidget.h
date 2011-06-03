#ifndef FILECOPYWIDGET_H
#define FILECOPYWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class FileCopyWidget;
}

namespace FileManagerPlugin {

class FileCopyTask;
class FileCopyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileCopyWidget(FileCopyTask *task, QWidget *parent = 0);
    ~FileCopyWidget();

signals:
    void canceled();

public slots:
    void update();
    void updateProgress(qint64 progress);
//    void showDetails(bool show);

private:
    Ui::FileCopyWidget *ui;
    FileCopyTask *m_task;
};

} // namespace FileManagerPlugin

#endif // FILECOPYWIDGET_H
