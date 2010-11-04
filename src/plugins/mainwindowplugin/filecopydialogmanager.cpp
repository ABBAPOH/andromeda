#include "filecopydialogmanager.h"

#include "filecopyreplacedialog.h"
#include "filecopywidget.h"
#include "filecopytask.h"
#include <filecopymanager.h>

using namespace MainWindowPlugin;
using namespace IO;

FileCopyDialogManager::FileCopyDialogManager(QObject *parent) :
    QObject(parent)
{
    FileCopyManager *manager = FileCopyManager::instance();
    connect(manager, SIGNAL(created(QtFileCopier*)), SLOT(addCopier(QtFileCopier*)));
}

void FileCopyDialogManager::addCopier(QtFileCopier *copier)
{
    connect(copier, SIGNAL(error(int,QtFileCopier::Error,bool)),
            SLOT(handleError(int,QtFileCopier::Error,bool)));

    FileCopyTask *task = new FileCopyTask(this);
    task->setCopier(copier);
    FileCopyWidget *widget = new FileCopyWidget(task);
    widget->show();

}

void FileCopyDialogManager::handleError(int id, QtFileCopier::Error error, bool stopped)
{
    if (error == QtFileCopier::DestinationExists) {
        QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
        FileCopyReplaceDialog *dialog = new FileCopyReplaceDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, SIGNAL(cancelAll()), copier, SLOT(cancelAll()));
        connect(dialog, SIGNAL(overwrite()), copier, SLOT(overwrite()));
        connect(dialog, SIGNAL(overwriteAll()), copier, SLOT(overwriteAll()));
        connect(dialog, SIGNAL(skip()), copier, SLOT(skip()));
        connect(dialog, SIGNAL(skipAll()), copier, SLOT(skipAll()));
        dialog->show();
    }
}
