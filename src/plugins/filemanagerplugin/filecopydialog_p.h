#ifndef FILECOPYDIALOG_P_H
#define FILECOPYDIALOG_P_H

#include <QtCore/QSet>
#include <QFileCopier>

#include "filecopydialog.h"

namespace FileManagerPlugin {

class FileCopyDialogPrivate: public QObject
{
    Q_OBJECT
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

} // namespace FileManagerPlugin

#endif // FILECOPYDIALOG_P_H
