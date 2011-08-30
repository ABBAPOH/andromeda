#ifndef FILECOPYTASK_H
#define FILECOPYTASK_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "QFileCopier"

class QTimeEvent;

namespace FileManagerPlugin {

class FileCopyTaskPrivate;
class FileCopyTask : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileCopyTask)
    Q_DISABLE_COPY(FileCopyTask)

public:
    explicit FileCopyTask(QObject *parent = 0);
    ~FileCopyTask();

    QFileCopier *copier() const;
    void setCopier(QFileCopier *copier);

    QString currentFilePath();

    int objectsCount() const;
    int totalObjects() const;

    int speed() const;

    int remainingTime() const;

    qint64 finishedSize() const;
    qint64 totalSize() const;

signals:
    void updated();
    void currentProgress(qint64 progress);
    void progress(qint64 progress);

protected:
    void timerEvent(QTimerEvent *);

    FileCopyTaskPrivate *d_ptr;

    Q_PRIVATE_SLOT(d_func(), void onStateChanged(QFileCopier::State state))
    Q_PRIVATE_SLOT(d_func(), void onStarted(int identifier))
    Q_PRIVATE_SLOT(d_func(), void onProgress(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onDone())
};

} // namespace FileManagerPlugin

#endif // FILECOPYTASK_H
