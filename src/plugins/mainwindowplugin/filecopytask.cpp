#include "filecopytask.h"

#include <QtCore/QFileInfo>

using namespace MainWindowPlugin;

FileCopyTask::FileCopyTask(QObject *parent) :
    QObject(parent),
    m_totalSize(0)
{
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
    m_copier = copier;
}

QString FileCopyTask::currentFilePath()
{
    return m_requests.value(m_copier->currentId()).source;
}

int FileCopyTask::objectsCount()
{
    return m_objectsCount - 1;
}

int FileCopyTask::totalSize()
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
        qDebug("started");
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
    emit updated();
}

void FileCopyTask::onProgress(int /*identifier*/, qint64 progress)
{
    m_copiedSize += progress;
    emit currentProgress(progress);
    emit this->progress(m_copiedSize);
}

void FileCopyTask::reset()
{
    // maybe we need to use threads to get full size in case of remote files?
    m_totalSize = 0;
    m_totalObjects = 0;
    m_objectsCount = 0;
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



