#ifndef FILECOPYTASK_H
#define FILECOPYTASK_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtFileCopier>

class QTimeEvent;
namespace MainWindowPlugin {

class Request
{
public:
    QString source;
    QString destination;
    int size;
};

class FileCopyTaskPrivate;
class FileCopyTask : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileCopyTask)
    Q_DISABLE_COPY(FileCopyTask)
public:
    explicit FileCopyTask(QObject *parent = 0);
    ~FileCopyTask();

    QtFileCopier *copier();
    void setCopier(QtFileCopier *copier);
    QString currentFilePath();
    qint64 finishedSize();
    int objectsCount();
    int remainingTime();
    int speed();
    int totalObjects();
    qint64 totalSize();

signals:
    void updated();
    void currentProgress(qint64 progress);
    void progress(qint64 progress);

protected:
    void timerEvent(QTimerEvent *);

    FileCopyTaskPrivate *d_ptr;

    Q_PRIVATE_SLOT(d_func(), void onStateChanged(QtFileCopier::State state))
    Q_PRIVATE_SLOT(d_func(), void onStarted(int identifier))
    Q_PRIVATE_SLOT(d_func(), void onProgress(int identifier, qint64 progress))
    Q_PRIVATE_SLOT(d_func(), void onDone())
};

}

#endif // FILECOPYTASK_H
