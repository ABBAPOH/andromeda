#ifndef FILESYSTEMUNDOMANAGER_P_H
#define FILESYSTEMUNDOMANAGER_P_H

#include "filemanagerplugin_global.h"

#include <QtCore/QObject>
#include <QtFileCopier>

class FILEMANAGERPLUGIN_EXPORT FileCopyManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FileCopyManager)
public:
    explicit FileCopyManager(QObject *parent = 0);
    ~FileCopyManager();

    static FileCopyManager *instance();

    QtFileCopier *copier();

signals:
    void created(QtFileCopier *);
    void destroyed(QtFileCopier *);

public slots:

private slots:
    void onDone(bool);

};

#endif // FILESYSTEMUNDOMANAGER_P_H
