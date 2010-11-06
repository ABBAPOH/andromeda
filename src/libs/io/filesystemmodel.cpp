#include "filesystemmodel.h"

#include "filecopymanager.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>

using namespace IO;

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
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
    QtFileCopier *copier = FileCopyManager::instance()->copier();

    switch (action) {
    case Qt::CopyAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            copier->copy(path, to + QFileInfo(path).fileName());
            success = true;
        }
        break;
    case Qt::LinkAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::link(path, to + QFileInfo(path).fileName()) && success;
        }
        break;
    case Qt::MoveAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            copier->move(path, to + QFileInfo(path).fileName());
            success = true;
        }
        break;
    default:
        return false;
    }

    return success;
}
