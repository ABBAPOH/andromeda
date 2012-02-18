#ifndef FILECOPYTASK_P_H
#define FILECOPYTASK_P_H

#include "filecopytask.h"

namespace FileManager {

class FileCopyTaskPrivate
{
    Q_DECLARE_PUBLIC(FileCopyTask)
    FileCopyTask *q_ptr;

public:
    explicit FileCopyTaskPrivate(FileCopyTask *qq);

    void reset();

    // slots:
    void onStateChanged(QFileCopier::State state);
    void onStarted(int);
    void onFinished(int);
    void onProgress(qint64 progress, qint64 progressTotal);
    void onDone();

    QFileCopier *copier;

    int objectsCount;
    int totalObjects;

    int speed;
    qint64 speedLastSize;

    qint64 currentProgress;
    qint64 finishedSize;
    qint64 totalSize;
};

} // namespace FileManager

#endif // FILECOPYTASK_P_H
