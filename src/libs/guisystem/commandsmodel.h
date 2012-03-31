#ifndef COMMANDSMODEL_H
#define COMMANDSMODEL_H

#include "guisystem_global.h"

#include <QtCore/QAbstractItemModel>

namespace GuiSystem {

class CommandsModelPrivate;
class GUISYSTEM_EXPORT CommandsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandsModel)
    Q_DISABLE_COPY(CommandsModel)

public:
    explicit CommandsModel(QObject *parent = 0);
    ~CommandsModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool isModified(const QModelIndex &index) const;

    void resetShortcut(const QModelIndex &index);

    bool exportShortcuts(QIODevice *device) const;
    bool importShortcuts(QIODevice *device);

protected:
    CommandsModelPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // COMMANDSMODEL_H
