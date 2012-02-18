#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

namespace FileManager {

class FileSystemManager;
class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = 0);

    FileSystemManager *fileSystemManager() const;
    void setFileSystemManager(FileSystemManager *manager);

    QVariant data(const QModelIndex &index, int role) const;
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent
                      );
protected:
    FileSystemManager *m_manager; // can be safely changed to d_ptr
};

} // namespace FileManager

#endif // FILESYSTEMMODEL_H
