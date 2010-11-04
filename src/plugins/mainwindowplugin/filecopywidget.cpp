#include "filecopywidget.h"
#include "ui_filecopywidget.h"

#include "filecopytask.h"

using namespace MainWindowPlugin;

FileCopyWidget::FileCopyWidget(FileCopyTask *task, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileCopyWidget),
    m_task(task)
{
    ui->setupUi(this);
    connect(m_task, SIGNAL(updated()), SLOT(update()));
    connect(m_task, SIGNAL(progress(qint64)), SLOT(updateProgress(qint64)));
}

FileCopyWidget::~FileCopyWidget()
{
    delete ui;
}

void FileCopyWidget::update()
{
    ui->progressBar->setMaximum(m_task->totalSize());
    ui->nameLabel->setText(m_task->currentFilePath());
    ui->nameLabel_2->setText(m_task->currentFilePath());
    ui->sizeLabel->setText(QString::number(m_task->totalSize()));
    ui->totalObjectsLabel->setText(QString::number(m_task->totalObjects()));
    ui->remaingObjectLabel->setText(
            QString::number(m_task->totalObjects() - m_task->objectsCount())
            );
}

void FileCopyWidget::updateProgress(qint64 progress)
{
    ui->progressBar->setValue(progress);
}

