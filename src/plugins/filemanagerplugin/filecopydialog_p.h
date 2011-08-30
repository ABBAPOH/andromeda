#ifndef FILECOPYDIALOG_P_H
#define FILECOPYDIALOG_P_H

#include <QtCore/QMap>

#include "filecopydialog.h"
#include "QFileCopier"

namespace FileManagerPlugin {

class FileCopyDialogPrivate: public QObject
{
    Q_OBJECT
public:
    FileCopyDialogPrivate(FileCopyDialog *qq) : QObject(), q_ptr(qq) {}
    void addWidget(QWidget *widget);

    Ui::FileCopyDialog *ui;

    QMap<QFileCopier*, QWidget*> mapToWidget;
    QMap<QFileCopier*, QObject*> mapToTask;
    FileSystemManager *manager;

    FileCopyDialog *q_ptr;

public slots:
    void addCopier(QFileCopier *);
    void addCopier(int index);
    void update();
    void handleError(int id, QFileCopier::Error error, bool stopped);
};

} // namespace FileManagerPlugin

#endif // FILECOPYDIALOG_P_H
