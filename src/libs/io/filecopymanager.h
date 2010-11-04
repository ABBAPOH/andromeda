#ifndef FILECOPYMANAGER_H
#define FILECOPYMANAGER_H

#include "io_global.h"

#include <QtCore/QObject>
#include <QtFileCopier>

namespace IO {

class FileCopyManagerPrivate;
class IO_EXPORT FileCopyManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileCopyManager)
    Q_DISABLE_COPY(FileCopyManager)
public:
    explicit FileCopyManager(QObject *parent = 0);
    ~FileCopyManager();

    static FileCopyManager *instance();

    QtFileCopier *copier();

signals:
    void started(QtFileCopier *copier);
    void created(QtFileCopier *copier);

public slots:

private slots:
    void onStateChanged(QtFileCopier::State state);
    void onDone(bool error);
    void error(int id, QtFileCopier::Error error, bool stopped);

protected:
    FileCopyManagerPrivate *d_ptr;
};

}

#endif // FILECOPYMANAGER_H
