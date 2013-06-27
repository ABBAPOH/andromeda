#include "cookiemodel.h"

#include <QtCore/QDateTime>

#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

#include <QtNetwork/QNetworkCookie>

CookieModel::CookieModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_cookieJar(0)
{
}

void CookieModel::setCookeJar(CookieJar *cookieJar)
{
    if (!cookieJar)
        return;

    if (m_cookieJar == cookieJar)
        return;

    if (m_cookieJar)
        disconnect(m_cookieJar, 0, this, 0);

    m_cookieJar = cookieJar;

    connect(m_cookieJar, SIGNAL(cookiesChanged()), this, SLOT(onCookiesChanged()));

    beginResetModel();
    m_cookieJar->load();
    endResetModel();
}

int CookieModel::columnCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : 6;
}

QVariant CookieModel::data(const QModelIndex &index, int role) const
{
    QList<QNetworkCookie> lst;
    if (m_cookieJar)
        lst = m_cookieJar->allCookies();

    if (index.row() < 0 || index.row() >= lst.size())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        const QNetworkCookie &cookie = lst.at(index.row());
        switch (index.column()) {
        case 0: return cookie.domain();
        case 1: return cookie.name();
        case 2: return cookie.path();
        case 3: return cookie.isSecure();
        case 4: return cookie.expirationDate();
        case 5: return cookie.value();
        default: return QVariant();
        }
        }
    case Qt::FontRole:{
        QFont font;
        font.setPointSize(10);
        return font;
        }
    }

    return QVariant();
}

QVariant CookieModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::SizeHintRole) {
        QFont font;
        font.setPointSize(10);
        QFontMetrics fm(font);
        int height = fm.height() + fm.height()/3;
        int width = fm.width(headerData(section, orientation, Qt::DisplayRole).toString());
        return QSize(width, height);
    }

    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();

        switch (section) {
        case 0: return tr("Website");
        case 1: return tr("Name");
        case 2: return tr("Path");
        case 3: return tr("Secure");
        case 4: return tr("Expires");
        case 5: return tr("Contents");
        default: return QVariant();
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

bool CookieModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || !m_cookieJar)
        return false;

    int lastRow = row + count - 1;
    beginRemoveRows(parent, row, lastRow);

    QList<QNetworkCookie> lst = m_cookieJar->allCookies();
    for (int i = lastRow; i >= row; --i) {
        lst.removeAt(i);
    }
    m_cookieJar->setAllCookies(lst);

    endRemoveRows();

    return true;
}

int CookieModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid() || !m_cookieJar) ? 0 : m_cookieJar->allCookies().count();
}

void CookieModel::onCookiesChanged()
{
    beginResetModel();
    endResetModel();
}
