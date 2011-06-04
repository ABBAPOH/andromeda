#include "filesystemmodel.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>

#include "filesystemundomanager.h"

using namespace FileManagerPlugin;

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent),
    m_undoManager(FileSystemUndoManager::instance())
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
        CopyCommand * command = new CopyCommand();
        command->setDestination(to);
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            command->appendSourcePath(path);
            success = true;
        }
        m_undoManager->undoStack()->push(command);
        break;
    }
    case Qt::LinkAction: {
        LinkCommand * command = new LinkCommand();
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            command->appendPaths(path, to + QFileInfo(path).fileName());
        }
        m_undoManager->undoStack()->push(command);
        break;
    }
    case Qt::MoveAction: {
        MoveCommand * command = new MoveCommand();
        command->setDestination(to);
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            command->appendSourcePath(path);
            success = true;
        }
        m_undoManager->undoStack()->push(command);
        break;
    }
    default:
        return false;
    }

    return success;
}
