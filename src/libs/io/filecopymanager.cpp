#include "filecopymanager.h"
#include "filecopymanager_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

using namespace IO;

FileCopyManager::FileCopyManager(QObject *parent) :
    QObject(parent),
    d_ptr(new FileCopyManagerPrivate())
{
    qDebug("FileCopyManager::FileCopyManager");
}

FileCopyManager::~FileCopyManager()
{
    delete d_ptr;
}

FileCopyManager *FileCopyManager::instance()
{
    static FileCopyManager *m_instance = new FileCopyManager(qApp);
    return m_instance;
}

QtFileCopier *FileCopyManager::copier()
{
    Q_D(FileCopyManager);

        QtFileCopier *copier = new QtFileCopier(this);
        connect(copier, SIGNAL(stateChanged(QtFileCopier::State)),
                SLOT(onStateChanged(QtFileCopier::State)));
        connect(copier, SIGNAL(done(bool)), SLOT(onDone(bool)));
        connect(copier, SIGNAL(error(int,QtFileCopier::Error,bool)),
                SLOT(error(int,QtFileCopier::Error,bool)));
        emit created(copier);
        return copier;
}

void FileCopyManager::onDone(bool error)
{
    Q_D(FileCopyManager);
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
    if (!copier)
        return;

    copier->deleteLater();
    qDebug() << "done, error:" << error;
}

void FileCopyManager::onStateChanged(QtFileCopier::State state)
{
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
    if (!copier)
        return;
    if (state == QtFileCopier::Busy && copier->state() == QtFileCopier::Idle) {
//        qDebug() << "started";
        emit started(copier);
    }
    qDebug() << state;
}

void FileCopyManager::error(int id, QtFileCopier::Error error, bool stopped)
{
    qDebug() << "error" << id << error << stopped;
//    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());
//copier->overwrite();
}

FileCopyManagerPrivate::FileCopyManagerPrivate()
{

}
