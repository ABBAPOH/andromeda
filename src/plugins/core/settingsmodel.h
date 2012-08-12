#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QtCore/QAbstractItemModel>

class QSettings;

namespace Core {

class SettingsModelPrivate;
class SettingsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SettingsModel)
    Q_DISABLE_COPY(SettingsModel)

    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
    Q_PROPERTY(EditStrategy editStrategy READ editStrategy WRITE setEditStrategy)
public:
    enum EditStrategy { OnFieldChange = 0, OnManualSubmit = 1 };
    Q_ENUMS(EditStrategy)

    explicit SettingsModel(QObject *parent = 0);
    ~SettingsModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QSettings *settings() const;
    void setSettings(QSettings *settings);

    EditStrategy editStrategy() const;
    void setEditStrategy(EditStrategy strategy);

    bool readOnly() const;
    void setReadOnly(bool readOnly);

public slots:
    void clear();
    void submitAll();
    void revertAll();
    void refresh();

protected:
    SettingsModelPrivate *d_ptr;
};

} // namespace Core

#endif // SETTINGSSMODEL_H
