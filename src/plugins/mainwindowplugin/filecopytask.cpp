#include "filecopytask.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTimerEvent>

using namespace MainWindowPlugin;

FileCopyTask::FileCopyTask(QObject *parent) :
    QObject(parent),
    m_copier(0),
    m_totalSize(0),
    m_finishedSize(0),
    m_objectsCount(0),
    m_speed(0),
    m_totalObjects(0)
{
    startTimer(1000);
}

FileCopyTask::~FileCopyTask()
{
}

QtFileCopier *FileCopyTask::copier()
{
    return m_copier;
}

void FileCopyTask::setCopier(QtFileCopier *copier)
{
    if (m_copier) {
        disconnect(m_copier, 0, this, 0);
    }
    if (m_copier != copier) {
        m_copier = copier;
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
    return m_requests.value(m_copier->currentId()).source;
}

qint64 FileCopyTask::finishedSize()
{
    return m_finishedSize;
}

int FileCopyTask::objectsCount()
{
    return m_objectsCount - 1;
}

int FileCopyTask::remainingTime()
{
    if (speed() == 0)
        return -1;
    return 1000*(qint64)(totalSize() - finishedSize())/speed();
}

int FileCopyTask::speed()
{
    return m_speed;
}

qint64 FileCopyTask::totalSize()
{
    return m_totalSize;
}

int FileCopyTask::totalObjects()
{
    return m_totalObjects;
}

void FileCopyTask::onStateChanged(QtFileCopier::State state)
{
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
    if (state == QtFileCopier::Busy && copier->state() == QtFileCopier::Idle) {
        reset();
    }
}

void FileCopyTask::onStarted(int identifier)
{
    Q_ASSERT(identifier == m_copier->currentId());

    Request request = m_requests.value(identifier);
    QFileInfo info(request.source);

    if (request.size != info.size()) {
        m_totalSize -= request.size;
        m_totalSize += info.size();
        request.size = info.size();
        m_requests.insert(identifier, request);
    }
    m_objectsCount++;
    m_currentProgress = 0;
    emit updated();
}

void FileCopyTask::onProgress(int /*identifier*/, qint64 progress)
{
    qint64 oldFinishedSize = m_finishedSize;
    m_finishedSize += progress - m_currentProgress;
    m_currentProgress = progress;

    emit currentProgress(progress);
    emit this->progress(m_finishedSize);
}

void FileCopyTask::onDone()
{
    m_finishedSize = m_totalSize;
    m_objectsCount = m_totalObjects + 1;

    emit currentProgress(0);
    emit this->progress(m_finishedSize);
    emit updated();
}

void FileCopyTask::reset()
{
    // maybe we need to use threads to get full size in case of remote files?
    m_totalSize = 0;
    m_finishedSize = 0;
    m_totalObjects = 0;
    m_objectsCount = 0;
    m_speedLastSize = 0;
    if (!m_copier)
        return;
    QList<int> pending = m_copier->pendingRequests();
    foreach (int id, pending) {
        Request request;
        request.source = m_copier->sourceFilePath(id);
        request.destination = m_copier->destinationFilePath(id);
        QFileInfo fileInfo(request.source);
        request.size = fileInfo.size();
        m_totalSize += request.size;
        m_requests.insert(id, request);
    }
    m_totalObjects = pending.size();
}

void FileCopyTask::timerEvent(QTimerEvent *e)
{
    m_speed = (qint64)(m_finishedSize - m_speedLastSize); // delta per second
    m_speedLastSize = m_finishedSize;
}



