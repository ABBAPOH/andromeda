#ifndef FILECOPYDIALOG_P_H
#define FILECOPYDIALOG_P_H

#include <QtCore/QSet>
#include <IO/QFileCopier>

#include "filecopydialog.h"

namespace FileManager {

class FileCopyDialogPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FileCopyDialog)

public:
    FileCopyDialogPrivate(FileCopyDialog *qq) : QObject(), q_ptr(qq) {}
    void addWidget(QWidget *widget);

    Ui::FileCopyDialog *ui;

    QSet<QFileCopier*> copiers;
    FileSystemManager *manager;

    FileCopyDialog *q_ptr;

public slots:
    void addCopier(QFileCopier *);
    void addCopier(int index);
    void onDone();
    void handleError(int id, QFileCopier::Error error, bool stopped);
};

} // namespace FileManager

#endif // FILECOPYDIALOG_P_H
