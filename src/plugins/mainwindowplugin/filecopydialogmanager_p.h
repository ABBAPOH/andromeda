#ifndef FILECOPYDIALOGMANAGER_P_H
#define FILECOPYDIALOGMANAGER_P_H

#include "filecopydialogmanager.h"

namespace MainWindowPlugin {

class FileCopyDialog;
class FileCopyDialogManagerPrivate
{
    Q_DECLARE_PUBLIC(FileCopyDialogManager)
    FileCopyDialogManager *q_ptr;
public:
    FileCopyDialogManagerPrivate(FileCopyDialogManager *qq);
    virtual ~FileCopyDialogManagerPrivate();

    FileCopyDialog *fileCopyDialog;
    QMap<QtFileCopier*, QWidget*> mapToWidget;
    QMap<QtFileCopier*, QObject*> mapToTask;

    void addCopier(QtFileCopier *);
    void handleError(int id, QtFileCopier::Error error, bool stopped);
    void update();
};

}

#endif // FILECOPYDIALOGMANAGER_P_H
