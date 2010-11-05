#ifndef FILECOPYTASK_P_H
#define FILECOPYTASK_P_H

#include "filecopytask.h"

namespace MainWindowPlugin {

class FileCopyTaskPrivate
{
    Q_DECLARE_PUBLIC(FileCopyTask)
public:
    FileCopyTaskPrivate(FileCopyTask *qq);

    void reset();

    // slots:
    void onStateChanged(QtFileCopier::State state);
    void onStarted(int identifier);
    void onProgress(int identifier, qint64 progress);
    void onDone();

    QtFileCopier *copier;
    qint64 finishedSize;
    qint64 currentProgress;
    int objectsCount;
    int speed;
    int totalObjects;
    qint64 totalSize;
    qint64 speedLastSize;
    QMap<int, Request> requests;

    FileCopyTask *q_ptr;
};

}

#endif // FILECOPYTASK_P_H
