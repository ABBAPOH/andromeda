#ifndef COOKIEEXCEPTIONSMODEL_H
#define COOKIEEXCEPTIONSMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QStringList>

#include "cookiejar.h"

class CookieExceptionsModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(CookieExceptionsModel)

public:
    enum CookieRule {
        Allow,
        AllowForSession,
        Block
    };
    Q_ENUMS(CookieRule)

    explicit CookieExceptionsModel(QObject *parent = 0);

    void addRule(QString host, CookieRule rule);

    inline CookieJar *cookieJar() const { return m_cookieJar; }
    void setCookeJar(CookieJar *cookieJar);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    void addHost(QString host, QStringList &add, QStringList &remove1, QStringList &remove2);

private:
    CookieJar *m_cookieJar;

    // Domains we allow, Domains we block, Domains we allow for this session
    QStringList m_allowedCookies;
    QStringList m_blockedCookies;
    QStringList m_sessionCookies;
};

#endif // COOKIEEXCEPTIONSMODEL_H
