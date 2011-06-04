#include "filesystemmodel.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>

#include "filesystemmanager.h"

using namespace FileManagerPlugin;

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent),
    m_manager(FileSystemManager::instance())
{
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
    QList<QUrl>::const_iterator it = urls.constBegin();

    switch (action) {
    case Qt::CopyAction: {
        QStringList files;
        foreach (const QUrl &url, urls) {
            QString path = url.toLocalFile();
            files.append(path);
            success = true;
        }
        m_manager->copyFiles(files, to);
        break;
    }
#warning "TODO: implement"
//    case Qt::LinkAction: {
//        LinkCommand * command = new LinkCommand();
//        for (; it != urls.constEnd(); ++it) {
//            QString path = (*it).toLocalFile();
//            command->appendPaths(path, to + QFileInfo(path).fileName());
//        }
//        m_manager->undoStack()->push(command);
//        break;
//    }
    case Qt::MoveAction: {
        QStringList files;
        foreach (const QUrl &url, urls) {
            QString path = url.toLocalFile();
            files.append(path);
            success = true;
        }
        m_manager->moveFiles(files, to);
        break;
    }
    default:
        return false;
    }

    return success;
}
