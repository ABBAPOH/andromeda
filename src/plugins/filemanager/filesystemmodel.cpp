#include "filesystemmodel.h"

#include <QtCore/QUrl>
#include <QtCore/QMimeData>

#include <QtGui/QApplication>
#include <QtGui/QImageReader>

#include <io/fileimageprovider.h>
#include <io/qdriveinfo.h>

#include "filesystemmanager.h"
#include "filethumbnails.h"

using namespace FileManager;

Q_GLOBAL_STATIC(FileImageProvider, imageProvider)

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent),
    m_manager(FileSystemManager::instance())
{
    setObjectName("FileSystemModel");

    thumbnails = new FileThumbnails(this);
    connect(thumbnails,SIGNAL(done(QString,QImage)),this,SLOT(onThumbDone(QString,QImage)));
    connect(this,SIGNAL(directoryLoaded(QString)),this,SLOT(loadThumbs(QString)));
}

FileSystemManager * FileSystemModel::fileSystemManager() const
{
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
    if (index.isValid() && index.column() == 0) {
        if (role == Qt::UserRole) {
            return imageProvider()->image(QFileInfo(filePath(index)), QSize(512, 512));
        } else if (role == Qt::DecorationRole) {
            QString path = filePath(index);
            if (thumbs.contains(path))
                return thumbs.value(path);
        }
    }

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
    QString to = filePath(parent) + '/';

    QList<QUrl> urls = data->urls();

    QStringList files;
    foreach (const QUrl &url, urls) {
        QString path = url.toLocalFile();
        if (QFileInfo(path).path() != filePath(parent))
            files.append(path);
    }

    if (files.isEmpty())
        return false;

    QDriveInfo srcDrive(filePath(parent));
    QDriveInfo dstDrive(files[0]);

    //default actions
    if (srcDrive != dstDrive)
        action = Qt::CopyAction;
    else
        action = Qt::MoveAction;

    Qt::KeyboardModifiers keyboardModifiers = QApplication::keyboardModifiers();
    //change action according to the keyboard modifier, if any.
    if (keyboardModifiers.testFlag(Qt::AltModifier))
        action = Qt::CopyAction;
    else if (keyboardModifiers.testFlag(Qt::ControlModifier))
        action = Qt::MoveAction;
    else if (keyboardModifiers.testFlag(Qt::ShiftModifier))
        action = Qt::LinkAction;

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

void FileSystemModel::onThumbDone(const QString &path, const QImage &thumb)
{
    thumbs.insert(path, QIcon(QPixmap::fromImage(thumb)));
    QModelIndex idx = index(path);
    emit dataChanged(idx, idx);
}

void FileSystemModel::loadThumbs(const QString &dir)
{
    QList<QByteArray> types = QImageReader::supportedImageFormats();
    QModelIndex dirIndex = index(dir);
    QStringList files;

    for (int row = 0; row < rowCount(dirIndex); row++) {
        QModelIndex idx = index(row, 0, dirIndex);
        QString path = filePath(idx);
        if (types.contains(QFileInfo(path).suffix().toUtf8()))
            files.append(path);
    }

    thumbnails->getThumbnails(files);
}
