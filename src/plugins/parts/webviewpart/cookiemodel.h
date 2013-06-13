#ifndef COOKIEMODEL_H
#define COOKIEMODEL_H

#include <QtCore/QAbstractItemModel>

#include "cookiejar.h"

class CookieModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CookieModel(QObject *parent = 0);

    inline CookieJar *cookieJar() const { return m_cookieJar; }
    void setCookeJar(CookieJar *cookieJar);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

private slots:
    void onCookiesChanged();

private:
    CookieJar *m_cookieJar;
};

#endif // COOKIEMODEL_H
