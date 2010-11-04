#ifndef FILECOPYDIALOGMANAGER_H
#define FILECOPYDIALOGMANAGER_H

#include <QtCore/QObject>
#include <QtFileCopier>

namespace MainWindowPlugin {

class FileCopyDialogManager : public QObject
{
    Q_OBJECT
public:
    explicit FileCopyDialogManager(QObject *parent = 0);

signals:

public slots:
    void addCopier(QtFileCopier *);
    void handleError(int id, QtFileCopier::Error error, bool stopped);
};

}

#endif // FILECOPYDIALOGMANAGER_H
