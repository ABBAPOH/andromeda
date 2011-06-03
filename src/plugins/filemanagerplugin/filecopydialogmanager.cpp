#include "filecopydialogmanager.h"
#include "filecopydialogmanager_p.h"

#include "filecopyreplacedialog.h"
#include "filecopytask.h"
#include "filecopydialog.h"
#include "filesystemundomanager_p.h"
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

using namespace FileManagerPlugin;

// ============= FileCopyDialogManager =============

FileCopyDialogManager::FileCopyDialogManager(QObject *parent) :
    QObject(parent),
    d_ptr(new FileCopyDialogManagerPrivate(this))
{
    FileCopyManager *manager = FileCopyManager::instance();
    connect(manager, SIGNAL(created(QtFileCopier*)), SLOT(addCopier(QtFileCopier*)));
}

FileCopyDialogManager::~FileCopyDialogManager()
{
    delete d_ptr;
}

void FileCopyDialogManager::addCopier(QtFileCopier *copier)
{
    Q_D(FileCopyDialogManager);
    connect(copier, SIGNAL(error(int,QtFileCopier::Error,bool)),
            SLOT(handleError(int,QtFileCopier::Error,bool)));

    d->addCopier(copier);
}

// ============= FileCopyDialogManagerPrivate =============

FileCopyDialogManagerPrivate::FileCopyDialogManagerPrivate(FileCopyDialogManager *qq):
        q_ptr(qq),
        fileCopyDialog(new FileCopyDialog)
{
}

FileCopyDialogManagerPrivate::~FileCopyDialogManagerPrivate()
{
    delete fileCopyDialog;
}

void FileCopyDialogManagerPrivate::addCopier(QtFileCopier *copier)
{
    FileCopyTask *task = new FileCopyTask();
    task->setCopier(copier);

    FileCopyWidget *widget = new FileCopyWidget(task);
    mapToTask.insert(copier, task);
    mapToWidget.insert(copier, widget);
    QObject::connect(copier, SIGNAL(done(bool)), q_func(), SLOT(update()));
    QObject::connect(widget, SIGNAL(canceled()), copier, SLOT(cancelAll()));

    fileCopyDialog->addWidget(widget);
//    QTimer::singleShot(2000, fileCopyDialog, SLOT(show()));
    fileCopyDialog->show();
}

void FileCopyDialogManagerPrivate::handleError(int id, QtFileCopier::Error error, bool stopped)
{
    Q_Q(FileCopyDialogManager);

    if (!stopped)
        return;
    if (error == QtFileCopier::DestinationExists) {
        QtFileCopier *copier = qobject_cast<QtFileCopier *>(q->sender());
        FileCopyReplaceDialog *dialog = new FileCopyReplaceDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QString destName = copier->destinationFilePath(id);
        dialog->setMessage(q->tr("%1 \"%2\" is already exists").
                           arg(copier->isDir(id) ? q->tr("Folder") : q->tr("File")).
                           arg(QFileInfo(destName).baseName())
                           );
        q->connect(dialog, SIGNAL(cancelAll()), copier, SLOT(cancelAll()));
        q->connect(dialog, SIGNAL(overwrite()), copier, SLOT(overwrite()));
        q->connect(dialog, SIGNAL(overwriteAll()), copier, SLOT(overwriteAll()));
        q->connect(dialog, SIGNAL(skip()), copier, SLOT(skip()));
        q->connect(dialog, SIGNAL(skipAll()), copier, SLOT(skipAll()));
        dialog->show();
    }
}

void FileCopyDialogManagerPrivate::update()
{
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(q_func()->sender());
    if (!copier)
        return;

    QObject * task = mapToTask.take(copier);
    delete task;
    QWidget *widget = mapToWidget.take(copier);
    delete widget;
    if (mapToTask.isEmpty())
        fileCopyDialog->hide();
}

#include "filecopydialogmanager.moc"
