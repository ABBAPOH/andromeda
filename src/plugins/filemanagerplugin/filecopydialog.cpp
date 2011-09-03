#include "filecopydialog.h"
#include "filecopydialog_p.h"

#include "filesystemmanager.h"
#include "filecopytask.h"
#include "filecopyreplacedialog.h"

#include <QtCore/QFileInfo>
#include <QtGui/QFileIconProvider>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

using namespace FileManagerPlugin;

namespace Ui {

class FileCopyDialog
{
public:
    QScrollArea *scrollArea;
    QVBoxLayout *layout;
    QWidget *widget;

    FileCopyDialog() {}

    void setupUi(QDialog *dialog)
    {
        dialog->resize(400, 400);
        layout = new QVBoxLayout;

        // We need this spacer to have space in bottom of list
        layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

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

void FileCopyDialogPrivate::addCopier(QFileCopier *copier)
{
    connect(copier, SIGNAL(error(int,QFileCopier::Error,bool)),
            SLOT(handleError(int,QFileCopier::Error,bool)));

    FileCopyTask *task = new FileCopyTask();
    task->setCopier(copier);
    mapToTask.insert(copier, task);

    FileCopyWidget *widget = new FileCopyWidget(task);
    mapToWidget.insert(copier, widget);
    QObject::connect(copier, SIGNAL(done(bool)), SLOT(update()));
    QObject::connect(widget, SIGNAL(canceled()), copier, SLOT(cancelAll()));

    addWidget(widget);
    q_ptr->show();
    q_ptr->raise();
}

void FileCopyDialogPrivate::addCopier(int index)
{
    FileSystemManager *manager = static_cast<FileSystemManager*>(sender());
    addCopier(manager->copier(index));
}

void FileCopyDialogPrivate::update()
{
    QFileCopier *copier = static_cast<QFileCopier *>(sender());
    if (!copier)
        return;

    QObject * task = mapToTask.take(copier);
    delete task;
    QWidget *widget = mapToWidget.take(copier);
    delete widget;
    if (mapToTask.isEmpty())
        q_ptr->hide();
}

void FileCopyDialogPrivate::handleError(int id, QFileCopier::Error error, bool stopped)
{
    if (!stopped)
        return;

    if (error == QFileCopier::DestinationExists) {
        QFileCopier *copier = static_cast<QFileCopier *>(sender());
        QFileInfo destInfo(copier->destinationFilePath(id));

        FileCopyReplaceDialog *dialog = new FileCopyReplaceDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setIcon(QFileIconProvider().icon(destInfo));
        dialog->setMessage(tr("%1 \"%2\" is already exists. Would you like to replace it?").
                           arg(copier->isDir(id) ? tr("Folder") : tr("File")).
                           arg(destInfo.fileName()));

        connect(dialog, SIGNAL(cancelAll()), copier, SLOT(skipAll()));
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
    d->manager = 0;
}

FileCopyDialog::FileCopyDialog(FileSystemManager *manager, QWidget *parent) :
    QDialog(parent),
    d_ptr(new FileCopyDialogPrivate(this))
{
    Q_D(FileCopyDialog);

    d->ui = new Ui::FileCopyDialog;
    d->ui->setupUi(this);
    d->manager = 0;

    setWindowTitle(tr("Copying"));
    setFileSystemManager(manager);
}

FileCopyDialog::~FileCopyDialog()
{
    delete d_ptr->ui;
    delete d_ptr;
}

FileSystemManager *FileCopyDialog::fileSystemManager() const
{
    return d_func()->manager;
}

void FileCopyDialog::setFileSystemManager(FileSystemManager *manager)
{
    Q_D(FileCopyDialog);

    if (d->manager) {
        disconnect(d->manager, 0, d, 0);
    }
    d->manager = manager;
    connect(d->manager, SIGNAL(started(int)), d, SLOT(addCopier(int)));
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

    update();
    updateProgress(0);
}

FileCopyWidget::~FileCopyWidget()
{
    delete ui;
}

static QString sizeToString(qint64 size)
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
    ui->nameLabel->setText(QFileInfo(m_task->currentFilePath()).fileName());
    ui->remaingObjectLabel->setText(QString("%1 / %2").
                                    arg(m_task->objectsCount()).
                                    arg(m_task->totalObjects()));
}

void FileCopyWidget::updateProgress(qint64 progress)
{
    qint64 finishedSize = progress;
    qint64 totalSize = m_task->totalSize();

    ui->sizeLabel->setText(QString("%1 / %2").
                           arg(sizeToString(finishedSize)).
                           arg(sizeToString(totalSize)));
    ui->progressBar->setValue(totalSize ? 100*finishedSize/totalSize : 0);

//#warning TODO:update speed and time on timer tick
    ui->speedLabel->setText(sizeToString(m_task->speed()));

    QTime time(0, 0);
    time = time.addMSecs(m_task->remainingTime());
    ui->timeLeftLabel->setText(time.toString("h:m:s"));
}
