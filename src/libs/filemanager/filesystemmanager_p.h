#ifndef FILESYSTEMMANAGER_P_H
#define FILESYSTEMMANAGER_P_H

#include "filesystemmanager.h"

#include <QtCore/QMap>

namespace FileManager {

class FileSystemManagerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FileSystemManager)

public:
    FileSystemManagerPrivate(FileSystemManager *qq) : q_ptr(qq) {}

    QUndoStack *undoStack;

    QList<FileSystemManager::FileOperation> operations;
    int currentIndex;
    QMap<int, QFileCopier*> mapToCopier;
    bool canUndo;
    bool canRedo;

    QFileCopier *copier(int index);
    void registerCopier(QFileCopier *copier, int index);
    int newOperation(FileSystemManager::FileOperationType type, const QStringList &files, const QString &destination);

    static void setOperationUndo(FileSystemManager::FileOperation &op, bool yes) { op.setUndo(yes); }

    FileSystemManager *q_ptr;

private slots:
    void onDone();
};

} // namespace FileManager

#endif // FILESYSTEMMANAGER_P_H
