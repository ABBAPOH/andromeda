#include "filesystemmodel.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <fileimageprovider.h>

#include "filesystemmanager.h"

using namespace FileManagerPlugin;

Q_GLOBAL_STATIC(FileImageProvider, imageProvider)

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent),
    m_manager(new FileSystemManager(this))
{
    setObjectName("FileSystemModel");
}

FileSystemManager * FileSystemModel::fileSystemManager() const
{
    return m_manager;
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

    switch (action) {
    case Qt::CopyAction: {
        m_manager->copy(files, to);
        break;
    }
    case Qt::LinkAction: {
        m_manager->link(files, to);
        break;
    }
    case Qt::MoveAction: {
        m_manager->move(files, to);
        break;
    }
    default:
        return false;
    }

    return success;
}
