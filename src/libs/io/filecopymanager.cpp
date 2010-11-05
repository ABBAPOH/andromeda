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
    connect(copier, SIGNAL(done(bool)), SLOT(onDone(bool)));
    emit created(copier);
    return copier;
}

void FileCopyManager::onDone(bool /*error*/)
{
    Q_D(FileCopyManager);
    QtFileCopier *copier = qobject_cast<QtFileCopier *>(sender());

    if (!copier)
        return;

    emit destroyed(copier);
    copier->deleteLater();
}

FileCopyManagerPrivate::FileCopyManagerPrivate()
{
}
