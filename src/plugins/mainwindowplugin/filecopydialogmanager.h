#ifndef FILECOPYDIALOGMANAGER_H
#define FILECOPYDIALOGMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtFileCopier>

namespace MainWindowPlugin {

class FileCopyDialog;
class FileCopyDialogManager : public QObject
{
    Q_OBJECT
public:
    explicit FileCopyDialogManager(QObject *parent = 0);
    ~FileCopyDialogManager();

signals:

public slots:
    void addCopier(QtFileCopier *);

private slots:
    void handleError(int id, QtFileCopier::Error error, bool stopped);
    void update();

private:
    FileCopyDialog *fileCopyDialog;
    QMap<QtFileCopier*, QWidget*> mapToWidget;
    QMap<QtFileCopier*, QObject*> mapToTask;
};

}

#endif // FILECOPYDIALOGMANAGER_H
