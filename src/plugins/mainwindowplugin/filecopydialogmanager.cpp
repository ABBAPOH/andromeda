#include "filecopydialogmanager.h"

#include "filecopyreplacedialog.h"
#include "filecopywidget.h"
#include "filecopytask.h"
#include "filecopydialog.h"
#include <filecopymanager.h>
#include <QtCore/QFileInfo>

using namespace MainWindowPlugin;
using namespace IO;

FileCopyDialogManager::FileCopyDialogManager(QObject *parent) :
    QObject(parent),
    fileCopyDialog(new FileCopyDialog)
{
    FileCopyManager *manager = FileCopyManager::instance();
    connect(manager, SIGNAL(created(QtFileCopier*)), SLOT(addCopier(QtFileCopier*)));
}

FileCopyDialogManager::~FileCopyDialogManager()
{
    delete fileCopyDialog;
}

void FileCopyDialogManager::addCopier(QtFileCopier *copier)
{
    connect(copier, SIGNAL(error(int,QtFileCopier::Error,bool)),
            SLOT(handleError(int,QtFileCopier::Error,bool)));

    FileCopyTask *task = new FileCopyTask(this);
    task->setCopier(copier);
    FileCopyWidget *widget = new FileCopyWidget(task);
    mapToTask.insert(copier, task);
    mapToWidget.insert(copier, widget);
    connect(copier, SIGNAL(done(bool)), SLOT(update()));
    connect(widget, SIGNAL(canceled()), copier, SLOT(cancelAll()));

    fileCopyDialog->addWidget(widget);
    fileCopyDialog->show();
}

void FileCopyDialogManager::handleError(int id, QtFileCopier::Error error, bool stopped)
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

void FileCopyDialogManager::update()
{
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
    if (!copier)
        return;

    QObject * task = mapToTask.take(copier);
    delete task;
    QWidget *widget = mapToWidget.take(copier);
    delete widget;
    if (mapToTask.isEmpty())
        fileCopyDialog->hide();
}
