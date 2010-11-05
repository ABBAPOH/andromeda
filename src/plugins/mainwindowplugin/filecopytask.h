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

class FileCopyTask : public QObject
{
    Q_OBJECT
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

private slots:
    void onStateChanged(QtFileCopier::State state);
    void onStarted(int identifier);
    void onProgress(int identifier, qint64 progress);
    void onDone();

private:
    QtFileCopier *m_copier;
    qint64 m_finishedSize;
    qint64 m_currentProgress;
    int m_objectsCount;
    int m_speed;
    int m_totalObjects;
    qint64 m_totalSize;
    qint64 m_speedLastSize;
    QMap<int, Request> m_requests;

    void reset();
protected:
    void timerEvent(QTimerEvent *);
};

}

#endif // FILECOPYTASK_H
