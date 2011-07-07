#ifndef FILECOPYDIALOG_P_H
#define FILECOPYDIALOG_P_H

#include <QtCore/QMap>

#include "filecopydialog.h"
#include "QtFileCopier"

namespace FileManagerPlugin {

class FileCopyDialogPrivate: public QObject
{
    Q_OBJECT
public:
    FileCopyDialogPrivate(FileCopyDialog *qq) : QObject(), q_ptr(qq) {}
    void addWidget(QWidget *widget);

    Ui::FileCopyDialog *ui;

    QMap<QtFileCopier*, QWidget*> mapToWidget;
    QMap<QtFileCopier*, QObject*> mapToTask;

    FileCopyDialog *q_ptr;

public slots:
    void addCopier(QtFileCopier *);
    void update();
    void handleError(int id, QtFileCopier::Error error, bool stopped);
};

} // namespace FileManagerPlugin

#endif // FILECOPYDIALOG_P_H
