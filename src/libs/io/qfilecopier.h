#ifndef QFILECOPIER_H
#define QFILECOPIER_H

#include "io_global.h"

#include <QtCore/QObject>

class QFileCopierPrivate;
class IO_EXPORT QFileCopier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progressInterval READ progressInterval WRITE setProgressInterval)
    Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)

public:
    explicit QFileCopier(QObject *parent = 0);
    ~QFileCopier();

    enum State {
        Idle,
        Gathering,
        Copying,
        WaitingForInteraction
    };
    Q_ENUMS(State)

    enum CopyFlag {
        NonInteractive = 0x01,
        Force = 0x02,
//        CancelOnError = 0x04,
        FollowLinks = 0x08, // if not set links are copied
        CopyOnMove = 0x10
    };

    enum Error {
        NoError,
        SourceNotExists,
        DestinationExists,
        CannotCreateDestinationDirectory,
        CannotOpenSourceFile,
        CannotOpenDestinationFile,
        CannotRemoveDestinationFile,
        CannotCreateSymLink,
        CannotReadSourceFile,
        CannotWriteDestinationFile,
        CannotRemoveSource,
        CannotRename,
        Canceled
    };

    Q_DECLARE_FLAGS(CopyFlags, CopyFlag)

    void copy(const QString &sourcePath, const QString &destinationPath, CopyFlags flags = 0);
    void copy(const QStringList &sourcePaths, const QString &destinationPath, CopyFlags flags = 0);

    void link(const QString &sourcePath, const QString &destinationPath, CopyFlags flags = 0);
    void link(const QStringList &sourcePaths, const QString &destinationPath, CopyFlags flags = 0);

    void move(const QString &sourcePath, const QString &destinationPath, CopyFlags flags = 0);
    void move(const QStringList &sourcePaths, const QString &destinationPath, CopyFlags flags = 0);

    void remove(const QString &path, CopyFlags flags = 0);
    void remove(const QStringList &paths, CopyFlags flags = 0);

    QList<int> pendingRequests() const;
    QList<int> topRequests() const;
    QString sourceFilePath(int id) const;
    QString destinationFilePath(int id) const;
    bool isDir(int id) const;
    QList<int> entryList(int id) const;
    int currentId() const;
    int count() const;
    qint64 size(int id) const;

    qint64 totalProgress() const;
    qint64 totalSize() const;

    State state() const;

    void setAutoReset(bool on);
    bool autoReset() const;
    int progressInterval() const;
    void setProgressInterval(int ms);

    void waitForFinished(unsigned long msecs = ULONG_MAX);

public slots:
    void cancelAll();
    void cancel(int id);

    void skip();
    void skipAll();
    void retry();

    void overwrite();
    void overwriteAll();

    void reset();
    void resetSkip();
    void resetOverwrite();

    void merge();
    void mergeAll();

signals:
    void error(int id, QFileCopier::Error error, bool stopped);

    void stateChanged(QFileCopier::State);

    void done(bool error);
    void started(int id);
    void progress(qint64 progress, qint64 size);
    void finished(int id, bool error);
    void canceled();

private:
    QFileCopierPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QFileCopier)
    Q_DISABLE_COPY(QFileCopier)
};

#endif // QFILECOPIER_H
