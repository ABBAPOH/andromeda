#ifndef NAVIGATIONMODEL_H
#define NAVIGATIONMODEL_H

#include "filemanager_global.h"

#include <QtCore/QAbstractItemModel>

#include <io/QDriveInfo>

namespace FileManager {

class NavigationModelPrivate;
class FILEMANAGER_EXPORT NavigationModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NavigationModel)
    Q_PROPERTY(StandardLocations standardLocations
               READ standardLocations
               WRITE setStandardLocations
               NOTIFY standardLocationsChanged)

public:
    enum StandardLocation {
        NoLocation = 0x00,
        DesktopLocation = 0x01,
        DocumentsLocation = 0x02,
        MusicLocation = 0x04,
        MoviesLocation = 0x08,
        PicturesLocation = 0x10,
        HomeLocation = 0x20,
        ApplicationsLocation = 0x40,
        DownloadsLocation = 0x080,
        StandardLocationCount = 0x080
    };

    Q_DECLARE_FLAGS(StandardLocations, StandardLocation)
    Q_FLAGS(StandardLocations)

    explicit NavigationModel(QObject *parent = 0);
    ~NavigationModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::DropActions supportedDropActions() const;

    QModelIndex index(const QString &path) const;
    QString path(const QModelIndex &index) const;

    void addFolder(const QString &path);
    void removeFolder(const QString &path);

    StandardLocations standardLocations() const;
    void setStandardLocation(StandardLocation loc, bool on);
    void setStandardLocations(StandardLocations locations);

    QDriveInfo driveInfo(const QModelIndex&) const;

signals:
    void standardLocationsChanged(NavigationModel::StandardLocations);
    void pathsDropped(const QString &parent, const QStringList &paths, Qt::DropAction action = Qt::CopyAction);

protected:
    NavigationModelPrivate *d_ptr;
};

} // namespace FileManager

#endif // NAVIGATIONMODEL_H
