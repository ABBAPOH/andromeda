#include "filesystemmodel.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <fileimageprovider.h>

#include "filesystemmanager.h"

using namespace FileManagerPlugin;

Q_GLOBAL_STATIC(FileImageProvider, imageProvider)

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent),
    m_manager(0)
{
    setObjectName("FileSystemModel");
}

FileSystemManager * FileSystemModel::fileSystemManager() const
{
    if (!m_manager) {
        FileSystemModel *self = const_cast<FileSystemModel*>(this);
        self->m_manager = new FileSystemManager(self);
    }
    return m_manager;
}

void FileSystemModel::setFileSystemManager(FileSystemManager *manager)
{
    if (!manager)
        return;

    if (m_manager == manager)
        return;

    if (m_manager && m_manager->parent() == this)
        delete m_manager;

    m_manager = manager;
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && index.column() == 0 && role == Qt::UserRole)
        return imageProvider()->image(QFileInfo(filePath(index)), QSize(512, 512));

    return QFileSystemModel::data(index, role);
}

bool FileSystemModel::dropMimeData(const QMimeData *data,
                  Qt::DropAction action,
                  int row,
                  int column,
                  const QModelIndex &parent
                  )
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!parent.isValid() || isReadOnly())
        return false;

    bool success = true;
    QString to = filePath(parent) + QDir::separator();

    QList<QUrl> urls = data->urls();

    QStringList files;
    foreach (const QUrl &url, urls) {
        QString path = url.toLocalFile();
        if (QFileInfo(path).path() != filePath(parent))
            files.append(path);
    }

    if (files.isEmpty())
        return false;

    FileSystemManager * manager = fileSystemManager();

    switch (action) {
    case Qt::CopyAction: {
        manager->copy(files, to);
        break;
    }
    case Qt::LinkAction: {
        manager->link(files, to);
        break;
    }
    case Qt::MoveAction: {
        manager->move(files, to);
        break;
    }
    default:
        return false;
    }

    return success;
}
