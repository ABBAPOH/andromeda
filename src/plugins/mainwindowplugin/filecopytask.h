#ifndef FILECOPYTASK_H
#define FILECOPYTASK_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtFileCopier>

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
    int objectsCount();
    int totalSize();
    int totalObjects();

signals:
    void updated();
    void currentProgress(qint64 progress);
    void progress(qint64 progress);

private slots:
    void onStateChanged(QtFileCopier::State state);
    void onStarted(int identifier);
    void onProgress(int identifier, qint64 progress);

private:
    QtFileCopier *m_copier;
    int m_totalSize;
    int m_copiedSize;
    int m_objectsCount;
    int m_totalObjects;
    QMap<int, Request> m_requests;

    void reset();
};

}

#endif // FILECOPYTASK_H
