#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

namespace FileManagerPlugin {

class FileSystemManager;
class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = 0);

protected:
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent
                      );
    FileSystemManager *m_manager; // can be safely changed to d_ptr
};

} // namespace FileManagerPlugin

#endif // FILESYSTEMMODEL_H
