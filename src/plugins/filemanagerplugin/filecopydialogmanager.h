#ifndef FILECOPYDIALOGMANAGER_H
#define FILECOPYDIALOGMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtFileCopier>

namespace MainWindowPlugin {

class FileCopyDialog;
class FileCopyDialogManagerPrivate;
class FileCopyDialogManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileCopyDialogManager)
    Q_DISABLE_COPY(FileCopyDialogManager)
public:
    explicit FileCopyDialogManager(QObject *parent = 0);
    ~FileCopyDialogManager();

signals:

public slots:
    void addCopier(QtFileCopier *);

protected:
    FileCopyDialogManagerPrivate *d_ptr;

    Q_PRIVATE_SLOT(d_func(), void update());
    Q_PRIVATE_SLOT(d_func(), void handleError(int id, QtFileCopier::Error error, bool stopped));
};

}

#endif // FILECOPYDIALOGMANAGER_H
