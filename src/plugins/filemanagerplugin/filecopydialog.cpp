#include "filecopydialog.h"
#include "filecopydialog_p.h"

#include "filesystemmanager.h"
#include "filecopytask.h"
#include "filecopyreplacedialog.h"

#include <QtCore/QFileInfo>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QResizeEvent>

using namespace FileManagerPlugin;

namespace Ui {

class FileCopyDialog
{
public:
    QScrollArea *scrollArea;
    QVBoxLayout *layout;
    QWidget *widget;

    FileCopyDialog(){}

    void setupUi(QDialog *dialog)
    {
        dialog->resize(400, 400);
        layout = new QVBoxLayout;

        layout->addSpacerItem(new QSpacerItem(0,
                                              0,
                                              QSizePolicy::Preferred,
                                              QSizePolicy::Expanding)
                              ); // We need this spacer to have space in bottom of list

        widget = new QWidget;
        widget->setLayout(layout);

        scrollArea = new QScrollArea(dialog);
        scrollArea->setWidgetResizable(true);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setWidget(widget);
    }
};

} // namespace Ui

void FileCopyDialogPrivate::addWidget(QWidget *widget)
{
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->layout->insertWidget(ui->layout->count() - 1, widget);
}

void FileCopyDialogPrivate::addCopier(QtFileCopier *copier)
{
    connect(copier, SIGNAL(error(int,QtFileCopier::Error,bool)),
            SLOT(handleError(int,QtFileCopier::Error,bool)));

//    d->addCopier(copier);
    FileCopyTask *task = new FileCopyTask();
    task->setCopier(copier);

    FileCopyWidget *widget = new FileCopyWidget(task);
//    mapToTask.insert(copier, task);
//    mapToWidget.insert(copier, widget);
    QObject::connect(copier, SIGNAL(done(bool)), SLOT(update()));
    QObject::connect(widget, SIGNAL(canceled()), copier, SLOT(cancelAll()));

    addWidget(widget);
//    QTimer::singleShot(2000, fileCopyDialog, SLOT(show()));
    q_ptr->show();
    q_ptr->raise();
}

void FileCopyDialogPrivate::update()
{
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
    if (!copier)
        return;

    QObject * task = mapToTask.take(copier);
    delete task;
    QWidget *widget = mapToWidget.take(copier);
    delete widget;
    if (mapToTask.isEmpty())
        q_ptr->hide();
}

void FileCopyDialogPrivate::handleError(int id, QtFileCopier::Error error, bool stopped)
{
    if (!stopped)
        return;
    if (error == QtFileCopier::DestinationExists) {
        QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
        FileCopyReplaceDialog *dialog = new FileCopyReplaceDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QString destName = copier->destinationFilePath(id);
        dialog->setMessage(tr("%1 \"%2\" is already exists").
                           arg(copier->isDir(id) ? tr("Folder") : tr("File")).
                           arg(QFileInfo(destName).baseName())
                           );
        connect(dialog, SIGNAL(cancelAll()), copier, SLOT(cancelAll()));
        connect(dialog, SIGNAL(overwrite()), copier, SLOT(overwrite()));
        connect(dialog, SIGNAL(overwriteAll()), copier, SLOT(overwriteAll()));
        connect(dialog, SIGNAL(skip()), copier, SLOT(skip()));
        connect(dialog, SIGNAL(skipAll()), copier, SLOT(skipAll()));
        dialog->show();
    }
}

FileCopyDialog::FileCopyDialog(QWidget *parent) :
    QDialog(parent),
    d_ptr(new FileCopyDialogPrivate(this))
{
    Q_D(FileCopyDialog);
    d->ui = new Ui::FileCopyDialog;
    d->ui->setupUi(this);

    FileSystemManager *manager = FileSystemManager::instance();
    connect(manager, SIGNAL(operationStarted(QtFileCopier*)), d, SLOT(addCopier(QtFileCopier*)));
}

FileCopyDialog::~FileCopyDialog()
{
    qDebug("FileCopyDialog");
    delete d_ptr->ui;
    delete d_ptr;
}

void FileCopyDialog::resizeEvent(QResizeEvent *e)
{
    d_ptr->ui->scrollArea->resize(e->size());
}

// FileCopyWidget

#include "ui_filecopywidget.h"

#include "filecopytask.h"

#include <QtCore/QTime>
#include <QtCore/QFileInfo>

using namespace FileManagerPlugin;

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

//#warning TODO:update speed and time on timer tick
    ui->speedLabel->setText(getStringSize(m_task->speed()));

    QTime time(0,0);
    time = time.addMSecs(m_task->remainingTime());
    ui->timeLeftLabel->setText(time.toString("h:m:s"));
}

