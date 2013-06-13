#include "cookieexceptionsmodel.h"

#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

CookieExceptionsModel::CookieExceptionsModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_cookieJar(0)
{
}

void CookieExceptionsModel::addRule(QString host, CookieRule rule)
{
    if (host.isEmpty())
        return;

    switch (rule) {
    case Allow :
        addHost(host, m_allowedCookies, m_blockedCookies, m_sessionCookies);
        break;
    case Block :
        addHost(host, m_blockedCookies, m_allowedCookies, m_sessionCookies);
        break;
    case AllowForSession :
        addHost(host, m_sessionCookies, m_allowedCookies, m_blockedCookies);
        break;
    default:
        break;
    }
}

void CookieExceptionsModel::setCookeJar(CookieJar *cookieJar)
{
    if (!cookieJar)
        return;

    if (m_cookieJar == cookieJar)
        return;

    if (m_cookieJar)
        disconnect(m_cookieJar, 0, this, 0);

    m_cookieJar = cookieJar;

    m_allowedCookies = m_cookieJar->allowedCookies();
    m_blockedCookies = m_cookieJar->blockedCookies();
    m_sessionCookies = m_cookieJar->allowForSessionCookies();
}

QVariant CookieExceptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::SizeHintRole) {
        QFont font;
        font.setPointSize(10);
        QFontMetrics fm(font);
        int height = fm.height() + fm.height()/3;
        int width = fm.width(headerData(section, orientation, Qt::DisplayRole).toString());
        return QSize(width, height);
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("Website");
        case 1: return tr("Status");
        default: break;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant CookieExceptionsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        int row = index.row();
        if (row < m_allowedCookies.count()) {
            switch (index.column()) {
            case 0: return m_allowedCookies.at(row);
            case 1: return tr("Allow");
            }
        }
        row = row - m_allowedCookies.count();
        if (row < m_blockedCookies.count()) {
            switch (index.column()) {
            case 0: return m_blockedCookies.at(row);
            case 1: return tr("Block");
            }
        }
        row = row - m_blockedCookies.count();
        if (row < m_sessionCookies.count()) {
            switch (index.column()) {
            case 0: return m_sessionCookies.at(row);
            case 1: return tr("Allow For Session");
            }
        }
    }
    case Qt::FontRole: {
        QFont font;
        font.setPointSize(10);
        return font;
    }
    }

    return QVariant();
}

int CookieExceptionsModel::columnCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : 2;
}

int CookieExceptionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_cookieJar)
        return 0;

    return m_allowedCookies.count() + m_blockedCookies.count() + m_sessionCookies.count();
}

bool CookieExceptionsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || !m_cookieJar)
        return false;

    int lastRow = row + count - 1;
    beginRemoveRows(parent, row, lastRow);

    for (int i = lastRow; i >= row; --i) {
        if (i < m_allowedCookies.count()) {
            m_allowedCookies.removeAt(row);
            continue;
        }
        i = i - m_allowedCookies.count();
        if (i < m_blockedCookies.count()) {
            m_blockedCookies.removeAt(row);
            continue;
        }
        i = i - m_blockedCookies.count();
        if (i < m_sessionCookies.count()) {
            m_sessionCookies.removeAt(row);
            continue;
        }
    }
    m_cookieJar->setAllowedCookies(m_allowedCookies);
    m_cookieJar->setBlockedCookies(m_blockedCookies);
    m_cookieJar->setAllowForSessionCookies(m_sessionCookies);

    endRemoveRows();

    return true;
}

void CookieExceptionsModel::addHost(QString host, QStringList &add, QStringList &remove1, QStringList &remove2)
{
    beginResetModel();

    if (!add.contains(host)) {
        add.append(host);
        remove1.removeOne(host);
        remove2.removeOne(host);
    }

    // avoid to have similar rules, with or without starting dot, eg "arora-browser.org" and ".arora-browser.org"
    // means the same domain.
    QString otherRule;
    if (host.startsWith(QLatin1Char('.'))) {
        otherRule = host.mid(1);
    } else {
        otherRule = QLatin1String(".") + host;
    }

    add.removeOne(otherRule);
    remove1.removeOne(otherRule);
    remove2.removeOne(otherRule);

    endResetModel();
}

