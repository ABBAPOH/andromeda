#include "filecopydialog.h"
#include "filecopydialog_p.h"

#include "filesystemmanager.h"
#include "filecopyerrordialog.h"
#include "filecopyreplacedialog.h"

#include <QtCore/QFileInfo>
#include <QtGui/QFileIconProvider>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

using namespace FileManager;

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

        widget = new QWidget(dialog);
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
    copiers.insert(copier);
    connect(copier, SIGNAL(error(int,QFileCopier::Error,bool)), SLOT(handleError(int,QFileCopier::Error,bool)));
    connect(copier, SIGNAL(done(bool)), SLOT(onDone()));

    FileCopyWidget *widget = new FileCopyWidget(copier);
    connect(copier, SIGNAL(done(bool)), widget, SLOT(deleteLater()));

    addWidget(widget);

    q_ptr->show();
    q_ptr->raise();
}

void FileCopyDialogPrivate::addCopier(int index)
{
    FileSystemManager *manager = static_cast<FileSystemManager*>(sender());
    addCopier(manager->copier(index));
}

void FileCopyDialogPrivate::onDone()
{
    QFileCopier *copier = static_cast<QFileCopier *>(sender());
    if (!copier)
        return;

    disconnect(copier, 0, this, 0);
    copiers.remove(copier);
    if (copiers.isEmpty())
        q_ptr->hide();
}
#include <QMetaEnum>
void FileCopyDialogPrivate::handleError(int id, QFileCopier::Error error, bool stopped)
{
    if (!stopped)
        return;

    QFileCopier *copier = static_cast<QFileCopier *>(sender());
    QFileInfo destInfo(copier->destinationFilePath(id));

    if (error == QFileCopier::DestinationExists) {
        FileCopyReplaceDialog *dialog = new FileCopyReplaceDialog(q_func());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setIcon(QFileIconProvider().icon(destInfo));
        dialog->setMessage(tr("%1 \"%2\" already exists. Would you like to replace it?").
                           arg(copier->isDir(id) ? tr("Folder") : tr("File")).
                           arg(destInfo.fileName()));

        connect(dialog, SIGNAL(cancelAll()), copier, SLOT(cancelAll()));
        connect(dialog, SIGNAL(overwrite()), copier, SLOT(overwrite()));
        connect(dialog, SIGNAL(overwriteAll()), copier, SLOT(overwriteAll()));
        connect(dialog, SIGNAL(skip()), copier, SLOT(skip()));
        connect(dialog, SIGNAL(skipAll()), copier, SLOT(skipAll()));

        dialog->setWindowModality(Qt::WindowModal);
        dialog->show();
    } else if (error == QFileCopier::DestinationAndSourceEqual) {
        copier->rename();
    } else {
        const QMetaObject *metaObject = copier->metaObject();
        int enumIndex = metaObject->indexOfEnumerator("Error");
        QMetaEnum metaEnum = metaObject->enumerator(enumIndex);

        FileCopyErrorDialog *dialog = new FileCopyErrorDialog(q_func());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setIcon(QFileIconProvider().icon(destInfo));
        dialog->setMessage(tr("Error occurred for %1 \"%2\": %3").
                           arg(copier->isDir(id) ? tr("Folder") : tr("File")).
                           arg(destInfo.fileName()).
                           arg(metaEnum.valueToKey(error)));

        connect(dialog, SIGNAL(abort()), copier, SLOT(cancelAll()));
        connect(dialog, SIGNAL(ignore()), copier, SLOT(skip()));
        connect(dialog, SIGNAL(retry()), copier, SLOT(retry()));
        dialog->setWindowModality(Qt::WindowModal);

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

    setFileSystemManager(FileSystemManager::instance());
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

using namespace FileManager;

FileCopyWidget::FileCopyWidget(QFileCopier *copier, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileCopyWidget)
{
    ui->setupUi(this);
    ui->groupBox->setHidden(true);

    m_task = new FileCopyTask(this);
    m_task->setCopier(copier);
    connect(m_task, SIGNAL(updated()), SLOT(update()));
    connect(m_task, SIGNAL(progress(qint64)), SLOT(updateProgress(qint64)));
    connect(ui->cancelButton, SIGNAL(clicked()), SIGNAL(canceled()));
    connect(this, SIGNAL(canceled()), copier, SLOT(cancelAll()));

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
