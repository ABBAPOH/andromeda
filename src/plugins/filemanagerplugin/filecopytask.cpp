#include "filecopytask.h"
#include "filecopytask_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTimerEvent>

using namespace FileManagerPlugin;

FileCopyTask::FileCopyTask(QObject *parent) :
    QObject(parent),
    d_ptr(new FileCopyTaskPrivate(this))
{
    startTimer(1000);
}

FileCopyTask::~FileCopyTask()
{
    delete d_ptr;
}

QtFileCopier *FileCopyTask::copier()
{
    return d_func()->copier;
}

void FileCopyTask::setCopier(QtFileCopier *copier)
{
    Q_D(FileCopyTask);

    if (d->copier) {
        disconnect(d->copier, 0, this, 0);
    }
    if (d->copier != copier) {
        d->copier = copier;
        connect(copier, SIGNAL(stateChanged(QtFileCopier::State)),
                SLOT(onStateChanged(QtFileCopier::State)));
        connect(copier, SIGNAL(started(int)), SLOT(onStarted(int)));
        connect(copier, SIGNAL(dataTransferProgress(int,qint64)),
                SLOT(onProgress(int,qint64)));
        connect(copier, SIGNAL(done(bool)), SLOT(onDone()));
    }
}

QString FileCopyTask::currentFilePath()
{
    Q_D(FileCopyTask);
    return d->requests.value(d->copier->currentId()).source;
}

qint64 FileCopyTask::finishedSize()
{
    return d_func()->finishedSize;
}

int FileCopyTask::objectsCount()
{
    return d_func()->objectsCount - 1;
}

int FileCopyTask::remainingTime()
{
    if (speed() == 0)
        return -1;
    return 1000*(qint64)(totalSize() - finishedSize())/speed();
}

int FileCopyTask::speed()
{
    return d_func()->speed;
}

qint64 FileCopyTask::totalSize()
{
    return d_func()->totalSize;
}

int FileCopyTask::totalObjects()
{
    return d_func()->totalObjects;
}

void FileCopyTask::timerEvent(QTimerEvent *e)
{
    Q_D(FileCopyTask);

    d->speed = (qint64)(d->finishedSize - d->speedLastSize); // delta per second
    d->speedLastSize = d->finishedSize;
}

// ============= FileCopyTaskPrivate =============

FileCopyTaskPrivate::FileCopyTaskPrivate(FileCopyTask *qq) :
    q_ptr(qq),
    copier(0),
    finishedSize(0),
    currentProgress(0),
    objectsCount(0),
    speed(0),
    totalObjects(0),
    totalSize(0),
    speedLastSize(0)
{
}

void FileCopyTaskPrivate::onStateChanged(QtFileCopier::State state)
{
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(q_func()->sender());
    if (state == QtFileCopier::Busy && copier->state() == QtFileCopier::Idle) {
        reset();
    }
}

void FileCopyTaskPrivate::onStarted(int identifier)
{
    Q_ASSERT(identifier == copier->currentId());

    Request request = requests.value(identifier);
    QFileInfo info(request.source);

    if (request.size != info.size()) {
        totalSize -= request.size;
        totalSize += info.size();
        request.size = info.size();
        requests.insert(identifier, request);
    }
    objectsCount++;
    currentProgress = 0;
    emit q_func()->updated();
}

void FileCopyTaskPrivate::onProgress(int /*identifier*/, qint64 progress)
{
    finishedSize += progress - currentProgress;
    currentProgress = progress;

    emit q_func()->currentProgress(progress);
    emit q_func()->progress(finishedSize);
}

void FileCopyTaskPrivate::onDone()
{
    finishedSize = totalSize;
    objectsCount = totalObjects + 1;

    emit q_func()->currentProgress(0);
    emit q_func()->progress(finishedSize);
    emit q_func()->updated();
}

void FileCopyTaskPrivate::reset()
{
    // maybe we need to use threads to get full size in case of remote files?
    totalSize = 0;
    finishedSize = 0;
    totalObjects = 0;
    objectsCount = 0;
    speedLastSize = 0;
    if (!copier)
        return;
    QList<int> pending = copier->pendingRequests();
    foreach (int id, pending) {
        Request request;
        request.source = copier->sourceFilePath(id);
        request.destination = copier->destinationFilePath(id);
        QFileInfo fileInfo(request.source);
        request.size = fileInfo.size();
        totalSize += request.size;
        requests.insert(id, request);
    }
    totalObjects = pending.size();
}

#include "filecopytask.moc"
