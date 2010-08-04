#ifndef PLUGINVIEWMODEL_H
#define PLUGINVIEWMODEL_H

#include "extensionsystem_global.h"

#include <QtCore/QAbstractItemModel>

namespace ExtensionSystem {

class PluginViewModelPrivate;
class EXTENSIONSYSTEM_EXPORT PluginViewModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginViewModel)
public:
    explicit PluginViewModel(QObject *parent = 0);
    ~PluginViewModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

signals:

public slots:

private:
    PluginViewModelPrivate * d_ptr;
};

} // namespace ExtensionSystem

#endif // PLUGINVIEWMODEL_H
