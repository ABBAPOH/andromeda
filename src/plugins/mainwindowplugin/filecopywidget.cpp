#include "filecopywidget.h"
#include "ui_filecopywidget.h"

#include "filecopytask.h"

#include <QtCore/QTime>
#include <QtCore/QFileInfo>

using namespace MainWindowPlugin;

FileCopyWidget::FileCopyWidget(FileCopyTask *task, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileCopyWidget),
    m_task(task)
{
    ui->setupUi(this);
    ui->groupBox->setHidden(true);

    connect(m_task, SIGNAL(updated()), SLOT(update()));
    connect(m_task, SIGNAL(progress(qint64)), SLOT(updateProgress(qint64)));
    connect(ui->cancelButton, SIGNAL(clicked()), SIGNAL(canceled()));
}

FileCopyWidget::~FileCopyWidget()
{
    delete ui;
}

QString getStringSize(qint64 size)
{
    if (size > (qint64)1024*1024*1024*1024)
        return QString::number(size/((qint64)1024*1024*1024*1024)) + "TB";
    if (size > 1024*1024*1024)
        return QString::number(size/(1024*1024*1024)) + "GB";
    if (size > 1024*1024)
        return QString::number(size/(1024*1024)) + "MB";
    if (size > 1024)
        return QString::number(size/1024) + "kB";
    return QString::number(size) + "b";
}

void FileCopyWidget::update()
{
    ui->progressBar->setMaximum(m_task->totalSize());
    QString fileName = QFileInfo(m_task->currentFilePath()).fileName();
    ui->nameLabel->setText(fileName);
    ui->nameLabel_2->setText(fileName);
    qint64 totalSize = m_task->totalSize();
    ui->sizeLabel->setText(getStringSize(totalSize));
    ui->totalObjectsLabel->setText(QString::number(m_task->totalObjects()));
    ui->remaingObjectLabel->setText(
            QString::number(m_task->totalObjects() - m_task->objectsCount())
            );
}

void FileCopyWidget::updateProgress(qint64 progress)
{
    qint64 totalSize = m_task->totalSize();
    qint64 finishedSize = m_task->finishedSize();
    ui->remainsLabel->setText(getStringSize(totalSize - finishedSize));
    ui->progressBar->setValue(progress);

#warning TODO:update speed and time on timer tick
    ui->speedLabel->setText(getStringSize(m_task->speed()));

    QTime time(0,0);
    time = time.addMSecs(m_task->remainingTime());
    ui->timeLeftLabel->setText(time.toString("h:m:s"));
}
