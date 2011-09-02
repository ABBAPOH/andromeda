#include "qfilecopier_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>

Q_DECLARE_METATYPE(QFileCopier::State)
Q_DECLARE_METATYPE(QFileCopier::Error)

static bool removePath(const QString &path)
{
    bool result = true;
    QFileInfo info(path);
    if (info.isDir()) {
        QDir dir(path);
        foreach (const QString &entry, dir.entryList(QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot)) {
            result &= removePath(dir.absoluteFilePath(entry));
        }
        if (!info.dir().rmdir(info.fileName()))
            return false;
    } else {
        result = QFile::remove(path);
    }
    return result;
}

QFileCopierThread::QFileCopierThread(QObject *parent) :
    QThread(parent),
    lock(QReadWriteLock::Recursive),
    m_currentId(-1),
    m_state(QFileCopier::Idle),
    shouldEmitProgress(false),
    waitingForInteraction(false),
    stopRequest(false),
    skipAllRequest(false),
    cancelAllRequest(false),
    overwriteAllRequest(false),
    mergeAllRequest(false),
    hasError(true),
    m_totalProgress(0),
    m_totalSize(0),
    autoReset(true)
{
}

QFileCopierThread::~QFileCopierThread()
{
    stopRequest = true;
    cancel();
    lock.lockForWrite();
    newCopyCondition.wakeOne();
    lock.unlock();
    wait();
}

void QFileCopierThread::enqueueTaskList(const QList<Task> &list)
{
    QWriteLocker l(&lock);
    taskQueue.append(list);
    restart();
}

QList<int> QFileCopierThread::pendingRequests(int id) const
{
    int size = 0;
    {
        QReadLocker l(&lock);
        size = requests.size();
    }
    QList<int> result;
    for ( ; id < size; id++) {
        result.append(id);
    }
    return result;
}

QList<int> QFileCopierThread::topRequests() const
{
    QReadLocker l(&lock);
    return topRequestsList;
}

QFileCopier::State QFileCopierThread::state() const
{
    return m_state;
}

void QFileCopierThread::setState(QFileCopier::State state)
{
    QWriteLocker l(&lock);

    m_state = state;
    emit stateChanged(m_state);
}

Request QFileCopierThread::request(int id) const
{
    QReadLocker l(&lock);
    return requests.value(id);
}

int QFileCopierThread::count() const
{
    QReadLocker l(&lock);
    return requests.size();
}

qint64 QFileCopierThread::totalProgress() const
{
    QReadLocker l(&lock);
    return m_totalProgress;
}

qint64 QFileCopierThread::totalSize() const
{
    QReadLocker l(&lock);
    return m_totalSize;
}

void QFileCopierThread::setAutoReset(bool on)
{
    QWriteLocker l(&lock);
    autoReset = on;
}

void QFileCopierThread::waitForFinished(unsigned long msecs)
{
    QWriteLocker l(&lock);
    waitForFinishedCondition.wait(&lock, msecs);
}

void QFileCopierThread::emitProgress()
{
    shouldEmitProgress = true;
}

void QFileCopierThread::cancel()
{
    QWriteLocker l(&lock);
    for (int id = 0; id < requests.size(); id++) {
        requests[id].canceled = true;
    }
    cancelAllRequest = true;
}

void QFileCopierThread::cancel(int id)
{
    QWriteLocker l(&lock);
    requests[id].canceled = true;
}

void QFileCopierThread::overwriteChildren(int id)
{
    Request &r = requests[id];
    r.overwrite = true;
    for (int i = 0; i < r.childRequests.size(); i++) {
        overwriteChildren(r.childRequests[i]);
    }
}

void QFileCopierThread::skip()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    requests[m_currentId].canceled = true;
    waitingForInteraction = false;
    interactionCondition.wakeOne();
}

void QFileCopierThread::skipAll()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    int id = m_currentId;
    requests[id].canceled = true;
    skipAllRequest = true;
    waitingForInteraction = false;
    interactionCondition.wakeOne();
}

void QFileCopierThread::overwrite()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    overwriteChildren(m_currentId);
    waitingForInteraction = false;
    interactionCondition.wakeOne();
}

void QFileCopierThread::overwriteAll()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    overwriteAllRequest = true;
    waitingForInteraction = false;
    interactionCondition.wakeOne();
}

void QFileCopierThread::retry()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    waitingForInteraction = false;
    interactionCondition.wakeOne();
}

void QFileCopierThread::resetSkip()
{
    QWriteLocker l(&lock);
    skipAllError.clear();
}

void QFileCopierThread::resetOverwrite()
{
    // not sure this is correct
    QWriteLocker l(&lock);
    overwriteAllRequest = true;
}

void QFileCopierThread::merge()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    int requestId = m_currentId;
    if (requests[requestId].isDir) {
        requests[requestId].merge = true;
        waitingForInteraction = false;
        interactionCondition.wakeOne();
    }
}

void QFileCopierThread::mergeAll()
{
    QWriteLocker l(&lock);
    if (!waitingForInteraction)
        return;

    mergeAllRequest = true;
    waitingForInteraction = false;
    interactionCondition.wakeOne();
}

void QFileCopierThread::run()
{
    bool stop = false;

    while (!stop) {
        lock.lockForWrite();

        if (cancelAllRequest) {
            cancelAllRequest = false;
            taskQueue.clear();
            requestQueue.clear();
            topRequestsList.clear();
            emit canceled();
            lock.unlock();
            continue;
        }

        if (taskQueue.isEmpty()) {
            if (requestQueue.isEmpty()) {
                if (stopRequest) {
                    lock.unlock();
                    stop = true;
                } else {
                    hasError = false;
                    setState(QFileCopier::Idle);
                    emit done(hasError);
                    waitForFinishedCondition.wakeOne();
                    if (autoReset) {
                        hasError = false;
                        overwriteAllRequest = false;
                        mergeAllRequest = false;
                        skipAllRequest = false;
                        skipAllError.clear();
                        topRequestsList.clear();
                    }
                    newCopyCondition.wait(&lock);
                    lock.unlock();
                }
            } else {
                lock.unlock();
                setState(QFileCopier::Copying);
                int id = requestQueue.takeFirst(); // inner queue, no lock
                handle(id);
            }
        } else {
            setState(QFileCopier::Gathering);
            Task t = taskQueue.takeFirst();
            lock.unlock();

            createRequest(t);
        }
    }
    deleteLater();
}

void QFileCopierThread::restart()
{
    QWriteLocker l(&lock);
    if (!isRunning()) {
        start();
    }
    newCopyCondition.wakeOne();
}

void QFileCopierThread::createRequest(Task t)
{
    QFileInfo sourceInfo(t.source);
    QFileInfo destInfo(t.dest);

    t.source = sourceInfo.absoluteFilePath();

    if ((destInfo.exists() && destInfo.isDir() && destInfo.fileName() != sourceInfo.fileName()) || t.dest.endsWith(QLatin1Char('/'))) {
        if (!destInfo.exists())
            QDir().mkpath(destInfo.absoluteFilePath());
        t.dest = destInfo.absoluteFilePath() + "/" + sourceInfo.fileName();
    } else {
        t.dest = destInfo.absoluteFilePath();
    }

    t.dest = QDir::cleanPath(t.dest);
    t.source = QDir::cleanPath(t.source);

#ifdef Q_OS_WIN
    if (t.type == Task::Link) {
        if (!t.dest.endsWith(QLatin1String(".lnk")))
            t.dest += QLatin1String(".lnk");
    }
#endif

    int index = addRequestToQueue(Request(t));
    if (index != -1) {
        requestQueue.append(index);
        topRequestsList.append(index);
    }
}

bool QFileCopierThread::shouldOverwrite(const Request &r)
{
    return r.overwrite || overwriteAllRequest || (r.copyFlags & QFileCopier::Force);
}

bool QFileCopierThread::shouldMerge(const Request &r)
{
    return r.merge || mergeAllRequest /*|| (r.copyFlags & QFileCopier::Merge)*/;
}

bool QFileCopierThread::checkRequest(int id)
{
    lock.lockForWrite();
    int parentId = m_currentId;
    m_currentId = id;
    lock.unlock();

    bool done = false;
    QFileCopier::Error err;
    while (!done) {
        Request r = request(id);
        err = QFileCopier::NoError;

        if (r.canceled) {
            done = true;
            err = QFileCopier::Canceled;
        } else if (!QFileInfo(r.source).exists()) {
            err = QFileCopier::SourceNotExists;
        } else if (!shouldOverwrite(r) && !shouldMerge(r) && QFileInfo(r.dest).exists()) {
            err = QFileCopier::DestinationExists;
        } else {
            done = true;
        }

        done = interact(id, r, done, err);
    }

    lock.lockForWrite();
    m_currentId = parentId;
    lock.unlock();

    return err == QFileCopier::NoError;
}

int QFileCopierThread::addRequestToQueue(Request request)
{
    int id = -1;

    {
        QWriteLocker l(&lock);
        id = requests.size();
        requests.append(request);
    }

    if (!checkRequest(id))
        return -1;

    {
        QWriteLocker l(&lock);
        request = requests[id]; // refresh request
    }

    QFileInfo sourceInfo(request.source);
    request.isDir = sourceInfo.isDir();
    request.size = request.isDir ? 0 : sourceInfo.size();

    {
        QWriteLocker l(&lock);

        m_totalSize += request.size;
        requests[id] = request; // update request
    }

    if (request.isDir) {
        if (request.type == Task::Move && !(request.copyFlags & QFileCopier::CopyOnMove)) {
            return id;
        }
        if (request.type == Task::Link) {
            return id;
        }

        QList<int> childRequests;

        QDirIterator i(request.source, QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
        while (i.hasNext()) {
            QString source = i.next();

            Request r;
            r.type = request.type;
            r.source = source;
            r.dest = request.dest + "/" + QFileInfo(source).fileName();
            r.copyFlags = request.copyFlags;

            int index = addRequestToQueue(r);
            if (index != -1)
                childRequests.append(index);
        }

        requests[id].childRequests = childRequests; // atomic assign
    }

    return id;
}

bool QFileCopierThread::interact(int id, const Request &r, bool done, QFileCopier::Error err)
{
    if (done || (r.copyFlags & QFileCopier::NonInteractive)) {
        done = true;
        if (err != QFileCopier::NoError)
            emit error(id, err, false);
    } else {
        lock.lockForWrite();
        if (stopRequest || skipAllError.contains(err)) {
            done = true;
            if (!stopRequest)
                emit error(id, err, false);
        } else {
            emit error(id, err, true);
            waitingForInteraction = true;
            interactionCondition.wait(&lock);
            if (skipAllRequest) {
                skipAllRequest = false;
                skipAllError.insert(err);
            }
        }
        lock.unlock();
    }
    return done;
}

/*!
  \internal

    Creates dir if necessary; throws error if fails
*/
bool QFileCopierThread::createDir(const Request &r, QFileCopier::Error *err)
{
//        if (!shouldMerge(r) && !QDir().mkpath(r.dest)) {
//            *err = QFileCopier::CannotCreateDestinationDirectory;
//            return false;
//        }
//        if (shouldMerge(r) && !QFileInfo(r.dest).exists() && !QDir().mkpath(r.dest)) {
//            *err = QFileCopier::CannotCreateDestinationDirectory;
//            return false;
//        }

    if (!(shouldMerge(r) && QFileInfo(r.dest).exists())) {
        if (!QDir().mkpath(r.dest)) {
            *err = QFileCopier::CannotCreateDestinationDirectory;
            return false;
        }
    }

    return true;
}

bool QFileCopierThread::copyFile(const Request &r, QFileCopier::Error *err)
{
    QString source = r.source;
    QFileInfo sourceInfo(source);

    if (sourceInfo.isSymLink()) {
        source = sourceInfo.symLinkTarget();
        if (!(r.copyFlags & QFileCopier::FollowLinks)) {
            if (!QFile::link(source, r.dest)) {
                *err = QFileCopier::CannotCreateSymLink;
                return false;
            }
            return true;
        }
    }

    QFile sourceFile(source);
    if (!sourceFile.open(QFile::ReadOnly)) {
        *err = QFileCopier::CannotOpenSourceFile;
        return false;
    }

    QFile destFile(r.dest);
    bool result = destFile.open(QFile::WriteOnly);
    if (!result) {
        *err = QFileCopier::CannotOpenDestinationFile;
        return false;
    }

    const int bufferSize = 4*1024; // 4 Kb
    QScopedArrayPointer<char> buffer(new char[bufferSize]);

    qint64 totalBytesWritten = 0;
    qint64 totalFileSize = sourceFile.size();
    qint64 prevTotalFileSize = totalFileSize;
    qint64 totalProgress = 0;

    qint64 lenRead = 0;
    do {

        if (r.canceled || cancelAllRequest) {
            *err = QFileCopier::Canceled;
            return false;
        }

        lenRead = sourceFile.read(buffer.data(), bufferSize);
        if (lenRead != 0) {

            if (lenRead == -1) {
                *err = QFileCopier::CannotReadSourceFile;
                return false;
            }

            qint64 lenWritten = 0;
            while (lenWritten < lenRead) {
                qint64 tmpLenWritten = destFile.write(buffer.data() + lenWritten, lenRead - lenWritten);
                if (tmpLenWritten == -1) {
                    *err = QFileCopier::CannotWriteDestinationFile;
                    return false;
                }
                lenWritten += tmpLenWritten;
            }

            totalBytesWritten += lenWritten;
            totalProgress += lenWritten;
            if (totalFileSize < totalBytesWritten) {
                totalFileSize = totalBytesWritten;
            }
        }

        if (shouldEmitProgress || lenRead == 0) { // we need to emit signal at end of loop
            {
                QWriteLocker l(&lock);
                requests[m_currentId].size = totalFileSize;
                m_totalSize += totalFileSize - prevTotalFileSize;
                m_totalProgress += totalProgress;
                totalProgress = 0;
                prevTotalFileSize = totalFileSize;
            }
            shouldEmitProgress = false;
            emit progress(totalBytesWritten, totalFileSize);
        }

    } while (lenRead != 0);

    return true;
}

bool QFileCopierThread::copy(const Request &r, QFileCopier::Error *err)
{
    if (r.isDir) {

        if (!createDir(r, err))
            return false;

        foreach (int id, r.childRequests) {
            handle(id);
        }

    } else {
        return copyFile(r, err);
    }

    return true;
}

bool QFileCopierThread::move(const Request &r, QFileCopier::Error *err)
{
    // mounted folders?
    bool result = true;
    if (r.copyFlags & QFileCopier::CopyOnMove) {

        if (r.isDir) {

            if (!createDir(r, err))
                return false;

            foreach (int id, r.childRequests) {
                handle(id);
            }

            if (!QDir().rmdir(r.source)) {
                *err = QFileCopier::CannotRemoveSource;
                return false;
            }

        } else {
            result = copyFile(r, err);
            if (result)
                result = remove(r, err);
        }

    } else {
        result = QFile::rename(r.source, r.dest);
        if (!result) {
            *err = QFileCopier::CannotRename;
        }
    }
    return result;
}

bool QFileCopierThread::link(const Request &r, QFileCopier::Error *err)
{
    bool result = QFile::link(r.source, r.dest);
    if (!result) {
        *err = QFileCopier::CannotCreateSymLink;
    }
    return result;
}

bool QFileCopierThread::remove(const Request &r, QFileCopier::Error *err)
{
    bool result = true;

    if (r.isDir) {

        foreach (int id, r.childRequests) {
            handle(id);
        }
        result = QDir().rmdir(r.source);

    } else {
        QFileInfo sourceInfo(r.source);
        if (sourceInfo.isSymLink() && (r.copyFlags & QFileCopier::FollowLinks)) {
            result &= QFile::remove(sourceInfo.symLinkTarget());
        }
        result &= QFile::remove(r.source);
    }

    if (!result) {
        *err = QFileCopier::CannotRemoveSource;
    }

    return result;
}

bool QFileCopierThread::processRequest(const Request &r, QFileCopier::Error *err)
{
    if (r.canceled) {
        *err = QFileCopier::Canceled;
        return true;
    }

// we skip this check to improve performance (if file removed after starting, we just coulnd't open/link it)
//    if (!QFileInfo(r.source).exists()) {
//        *err = QFileCopier::SourceNotExists;
//        return false;
//    }

    if (shouldOverwrite(r)) {
        if (QFileInfo(r.dest).exists()) {
            bool result = removePath(r.dest);
            if (!result) {
                *err = QFileCopier::CannotRemoveDestinationFile;
                return false;
            }
        }
    }

    switch (r.type) {
    case Task::Copy :
        return copy(r, err);
    case Task::Move :
        return move(r, err);
    case Task::Link :
        return link(r, err);
    case Task::Remove :
        return remove(r, err);
    default:
        break;
    }

    return true;
}

void QFileCopierThread::handle(int id)
{
    int parentId = m_currentId;
    {
        QWriteLocker l(&lock);
        emit started(id);
        m_currentId = id;
    }

    bool done = false;
    QFileCopier::Error err = QFileCopier::NoError;
    while (!done) {
        Request r = request(id);
        done = processRequest(r, &err);
        done = interact(id, r, done, err);
    }

    if (err != QFileCopier::NoError)
        hasError = true;

    {
        QWriteLocker l(&lock);
        m_currentId = parentId;
        emit finished(id);
    }
}

void QFileCopierPrivate::enqueueOperation(Task::Type operationType, const QStringList &sourcePaths,
                                          const QString &destinationPath, QFileCopier::CopyFlags flags)
{
    QList<Task> taskList;
    taskList.reserve(sourcePaths.size());
    foreach (const QString &path, sourcePaths) {
        Task t;
        t.source = path;
        t.dest = destinationPath;
        t.copyFlags = flags;
        t.type = operationType;
        taskList.append(t);
    }
    thread->enqueueTaskList(taskList);

    setState(QFileCopier::Copying);
}

void QFileCopierPrivate::onStarted(int id)
{
    requestStack.push(id);
    emit q_func()->started(id);
}

void QFileCopierPrivate::onFinished(int id)
{
    requestStack.pop();
    emit q_func()->finished(id, false);
}

void QFileCopierPrivate::onThreadFinished()
{
    setState(QFileCopier::Idle);
}

void QFileCopierPrivate::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == progressTimerId) {
        thread->emitProgress();
    }
}

/*!
    \class QFileCopier

    \brief The QFileCopier class allows you to copy and move files in a background process.
*/
QFileCopier::QFileCopier(QObject *parent) :
    QObject(parent),
    d_ptr(new QFileCopierPrivate(this))
{
    Q_D(QFileCopier);

    qRegisterMetaType <QFileCopier::State> ("QFileCopier::State");
    qRegisterMetaType <QFileCopier::Error> ("QFileCopier::Error");

    d->thread = new QFileCopierThread(this);
    connect(d->thread, SIGNAL(stateChanged(QFileCopier::State)), SIGNAL(stateChanged(QFileCopier::State)));
    connect(d->thread, SIGNAL(started(int)), d, SLOT(onStarted(int)));
    connect(d->thread, SIGNAL(finished(int)), d, SLOT(onFinished(int)));
    connect(d->thread, SIGNAL(progress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
    connect(d->thread, SIGNAL(error(int, QFileCopier::Error,bool)), SIGNAL(error(int, QFileCopier::Error,bool)));
    connect(d->thread, SIGNAL(done(bool)), d, SLOT(onThreadFinished()));
    connect(d->thread, SIGNAL(done(bool)), SIGNAL(done(bool)));
    d->state = Idle;

    d->progressInterval = 500;
    d->progressTimerId = d->startTimer(d->progressInterval);
    d->autoReset = true;
}

QFileCopier::~QFileCopier()
{
    delete d_ptr;
}

void QFileCopier::copy(const QString &sourcePath, const QString &destinationPath, CopyFlags flags)
{
    copy(QStringList() << sourcePath, destinationPath, flags);
}

void QFileCopier::copy(const QStringList &sourcePaths, const QString &destinationPath, CopyFlags flags)
{
    d_func()->enqueueOperation(Task::Copy, sourcePaths, destinationPath, flags);
}

void QFileCopier::link(const QString &sourcePath, const QString &destinationPath, CopyFlags flags)
{
    link(QStringList() << sourcePath, destinationPath, flags);
}

void QFileCopier::link(const QStringList &sourcePaths, const QString &destinationPath, CopyFlags flags)
{
    d_func()->enqueueOperation(Task::Link, sourcePaths, destinationPath, flags);
}

void QFileCopier::move(const QString &sourcePath, const QString &destinationPath, CopyFlags flags)
{
    move(QStringList() << sourcePath, destinationPath, flags);
}

void QFileCopier::move(const QStringList &sourcePaths, const QString &destinationPath, CopyFlags flags)
{
    d_func()->enqueueOperation(Task::Move, sourcePaths, destinationPath, flags);
}

void QFileCopier::remove(const QString &path, CopyFlags flags)
{
    remove(QStringList() << path, flags);
}

void QFileCopier::remove(const QStringList &paths, CopyFlags flags)
{
    d_func()->enqueueOperation(Task::Remove, paths, QString(), flags);
}

QList<int> QFileCopier::pendingRequests() const
{
    return d_func()->thread->pendingRequests(currentId());
}

QList<int> QFileCopier::topRequests() const
{
    return d_func()->thread->topRequests();
}

QString QFileCopier::sourceFilePath(int id) const
{
    return d_func()->thread->request(id).source;
}

QString QFileCopier::destinationFilePath(int id) const
{
    return d_func()->thread->request(id).dest;
}

bool QFileCopier::isDir(int id) const
{
    return d_func()->thread->request(id).isDir;
}

QList<int> QFileCopier::entryList(int id) const
{
    return d_func()->thread->request(id).childRequests;
}

int QFileCopier::currentId() const
{
    Q_D(const QFileCopier);
    if (d->requestStack.isEmpty())
        return -1;

    return d->requestStack.top();
}

int QFileCopier::count() const
{
    return d_func()->thread->count();
}

qint64 QFileCopier::size(int id) const
{
    return d_func()->thread->request(id).size;
}

qint64 QFileCopier::totalProgress() const
{
    return d_func()->thread->totalProgress();
}

qint64 QFileCopier::totalSize() const
{
    return d_func()->thread->totalSize();
}

QFileCopier::State QFileCopier::state() const
{
    return d_func()->state;
}

bool QFileCopier::autoReset() const
{
    return d_func()->autoReset;
}

void QFileCopier::setAutoReset(bool on)
{
    Q_D(QFileCopier);
    d->autoReset = on;
    d->thread->setAutoReset(on);
}

void QFileCopierPrivate::setState(QFileCopier::State s)
{
    if (state != s) {
        state = s;
        emit q_func()->stateChanged(state);
    }
}

int QFileCopier::progressInterval() const
{
    return d_func()->progressInterval;
}

void QFileCopier::setProgressInterval(int ms)
{
    Q_D(QFileCopier);

    if (d->progressInterval != ms) {
        d->killTimer(d->progressTimerId);
        d->progressInterval = ms;
        d->progressTimerId = d->startTimer(d->progressInterval);
    }
}

void QFileCopier::waitForFinished(unsigned long msecs)
{
    d_func()->thread->waitForFinished(msecs);
}

void QFileCopier::cancelAll()
{
    d_func()->thread->cancel();
}

void QFileCopier::cancel(int id)
{
    d_func()->thread->cancel(id);
}

void QFileCopier::skip()
{
    d_func()->thread->skip();
}

void QFileCopier::skipAll()
{
    d_func()->thread->skipAll();
}

void QFileCopier::retry()
{
    d_func()->thread->retry();
}

void QFileCopier::overwrite()
{
    d_func()->thread->overwrite();
}

void QFileCopier::overwriteAll()
{
    d_func()->thread->overwriteAll();
}

void QFileCopier::reset()
{
    resetSkip();
    resetOverwrite();
}

void QFileCopier::resetSkip()
{
    d_func()->thread->resetSkip();
}

void QFileCopier::resetOverwrite()
{
    d_func()->thread->resetOverwrite();
}

void QFileCopier::merge()
{
    d_func()->thread->merge();
}

void QFileCopier::mergeAll()
{
    d_func()->thread->mergeAll();
}
