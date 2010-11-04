#ifndef FILECOPYWIDGET_H
#define FILECOPYWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
    class FileCopyWidget;
}

namespace MainWindowPlugin {

class FileCopyTask;
class FileCopyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileCopyWidget(FileCopyTask *task, QWidget *parent = 0);
    ~FileCopyWidget();

public slots:
    void update();
    void updateProgress(qint64 progress);

private:
    Ui::FileCopyWidget *ui;
    FileCopyTask *m_task;
};

}

#endif // FILECOPYWIDGET_H
