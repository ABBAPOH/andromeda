#ifndef NAVIGATIONMODEL_H
#define NAVIGATIONMODEL_H

#include <QAbstractItemModel>

class NavigationModelPrivate;
class NavigationModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NavigationModel)
public:
    enum StandardLocation {
        NoLocation = 0x00,
        DesktopLocation = 0x01,
        DocumentsLocation = 0x02,
        MusicLocation = 0x04,
        MoviesLocation = 0x08,
        PicturesLocation = 0x10,
        HomeLocation = 0x20,
        ApplicationsLocation = 0x40
    };

    Q_DECLARE_FLAGS(StandardLocations, StandardLocation)
    Q_FLAGS(StandardLocations)

    explicit NavigationModel(QObject *parent = 0);
    ~NavigationModel();

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QString path(const QModelIndex &index) const;

    void addFolder(const QString &path);
    void removeFolder(const QString &path);

    StandardLocations standardLocations() const;
    void setStandardLocations(StandardLocations locations);

signals:

public slots:

protected:
    NavigationModelPrivate *d_ptr;
};

#endif // NAVIGATIONMODEL_H
