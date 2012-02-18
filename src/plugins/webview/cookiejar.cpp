#include "cookiejar.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtGui/QDesktopServices>

#include <QtWebKit/QWebSettings>

static const unsigned int JAR_VERSION = 23;

static const qint32 cookieJarMagic = 0xc00c1e;
static const qint8 cookieJarVersion = 1;

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
    stream << JAR_VERSION;
    stream << quint32(list.size());
    for (int i = 0; i < list.size(); ++i)
        stream << list.at(i).toRawForm();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
    list.clear();

    quint32 version;
    stream >> version;

    if (version != JAR_VERSION)
        return stream;

    quint32 count;
    stream >> count;
    for(quint32 i = 0; i < count; ++i)
    {
        QByteArray value;
        stream >> value;
        QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
        if (newCookies.count() == 0 && value.length() != 0) {
            qWarning() << "CookieJar: Unable to parse saved cookie:" << value;
        }
        for (int j = 0; j < newCookies.count(); ++j)
            list.append(newCookies.at(j));
        if (stream.atEnd())
            break;
    }
    return stream;
}
QT_END_NAMESPACE

CookieJar::CookieJar(QObject *parent) :
    QNetworkCookieJar(parent),
    m_loaded(false),
    m_saved(true),
    m_acceptCookies(AcceptOnlyFromSitesNavigatedTo),
    m_sessionLength(-1),
    m_filterTrackingCookies(false)
{
}

CookieJar::~CookieJar()
{
    if (m_keepCookies == KeepUntilExit)
        clear();

    save();
}

CookieJar::AcceptPolicy CookieJar::acceptPolicy() const
{
    ensureLoaded();

    return m_acceptCookies;
}

void CookieJar::setAcceptPolicy(AcceptPolicy policy)
{
    ensureLoaded();

    if (policy == m_acceptCookies)
        return;

    m_acceptCookies = policy;

    m_saved = false;
}

QStringList CookieJar::allowedCookies() const
{
    ensureLoaded();

    return m_exceptions_allow;
}

void CookieJar::setAllowedCookies(const QStringList &list)
{
    ensureLoaded();

    m_exceptions_allow = list;
    qSort(m_exceptions_allow.begin(), m_exceptions_allow.end());

    m_saved = false;
}

QStringList CookieJar::allowForSessionCookies() const
{
    ensureLoaded();

    return m_exceptions_allowForSession;
}

void CookieJar::setAllowForSessionCookies(const QStringList &list)
{
    ensureLoaded();

    m_exceptions_allowForSession = list;
    qSort(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end());

    m_saved = false;
}

QStringList CookieJar::blockedCookies() const
{
    ensureLoaded();

    return m_exceptions_block;
}

void CookieJar::setBlockedCookies(const QStringList &list)
{
    ensureLoaded();

    m_exceptions_block = list;
    qSort(m_exceptions_block.begin(), m_exceptions_block.end());

    m_saved = false;
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl &url) const
{
    ensureLoaded();

    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled)) {
        QList<QNetworkCookie> noCookies;
        return noCookies;
    }

    return QNetworkCookieJar::cookiesForUrl(url);
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    ensureLoaded();

    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return false;

    QString host = url.host();
    bool eBlock = qBinaryFind(m_exceptions_block.begin(), m_exceptions_block.end(), host) != m_exceptions_block.end();
    bool eAllow = qBinaryFind(m_exceptions_allow.begin(), m_exceptions_allow.end(), host) != m_exceptions_allow.end();
    bool eAllowSession = qBinaryFind(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end(), host) != m_exceptions_allowForSession.end();

    bool addedCookies = false;
    // pass exceptions
    bool acceptInitially = (m_acceptCookies != AcceptNever);
    if ((acceptInitially && !eBlock) || (!acceptInitially && (eAllow || eAllowSession))) {
        // pass url domain == cookie domain
        QDateTime soon = QDateTime::currentDateTime();
        soon = soon.addDays(90);
        foreach (QNetworkCookie cookie, cookieList) {

            if (cookie.isSessionCookie() && m_sessionLength != -1) {
                QDateTime now = QDateTime::currentDateTime();
                cookie.setExpirationDate(now.addDays(m_sessionLength));
            }

            if (!(m_filterTrackingCookies && cookie.name().startsWith("__utm"))) {
#if 0
                    qDebug() << "Cookie treated as tracking cookie" << cookie;
#endif
                continue;
            }

            QList<QNetworkCookie> lst;
            if (m_keepCookies == KeepUntilTimeLimit
                && !cookie.isSessionCookie()
                && cookie.expirationDate() > soon) {
                    cookie.setExpirationDate(soon);
            }
            lst += cookie;
            if (QNetworkCookieJar::setCookiesFromUrl(lst, url)) {
                addedCookies = true;
            } else {
                // finally force it in if wanted
                if (m_acceptCookies == AcceptAlways) {
                    QList<QNetworkCookie> cookies = allCookies();
                    cookies += cookie;
                    setAllCookies(cookies);
                    addedCookies = true;
                }
#if 0
                else
                    qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();
#endif
            }

        }
    }

    if (addedCookies) {
        m_saved = false;
        emit cookiesChanged();
    }

    return addedCookies;
}

CookieJar::KeepPolicy CookieJar::keepPolicy() const
{
    ensureLoaded();

    return m_keepCookies;
}

void CookieJar::setKeepPolicy(KeepPolicy policy)
{
    ensureLoaded();

    if (policy == m_keepCookies)
        return;

    m_keepCookies = policy;

    m_saved = false;
}

int CookieJar::sessionLength() const
{
    return m_sessionLength;
}

void CookieJar::setSessionLength(int sessionLength)
{
    m_sessionLength = sessionLength;
}

bool CookieJar::filterTrackingCookies() const
{
    return m_filterTrackingCookies;
}

void CookieJar::setFilterTrackingCookies(bool filter)
{
    m_filterTrackingCookies = filter;
}

void CookieJar::clear()
{
    setAllCookies(QList<QNetworkCookie>());

    m_saved = false;
    emit cookiesChanged();
}

void CookieJar::setAllCookies(const QList<QNetworkCookie> &cookieList)
{
    QNetworkCookieJar::setAllCookies(cookieList);
    m_saved = false;
    emit cookiesChanged();
}

void CookieJar::ensureLoaded()
{
    if (!m_loaded)
        load();
}

void CookieJar::ensureLoaded() const
{
    if (!m_loaded)
        (const_cast<CookieJar*>(this))->load();
}

void CookieJar::load()
{
    if (m_loaded)
        return;

    m_loaded = true;

    loadCookies();
    loadSettings();
}

void CookieJar::loadCookies()
{
    // load cookies and exceptions
    QFile f(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/cookies.data"));
    if (!f.open(QFile::ReadOnly))
        return;

    if (f.size() == 0)
        return;

    QDataStream s(&f);

    qint32 magic;
    qint8 version;
    QList<QNetworkCookie> cookies;

    s >> magic;
    if (magic != cookieJarMagic)
        return;

    s >> version;
    if (version != cookieJarVersion)
        return;

    s >> cookies;
    s >> m_exceptions_block;
    s >> m_exceptions_allow;
    s >> m_exceptions_allowForSession;
    setAllCookies(cookies);

    qSort(m_exceptions_block.begin(), m_exceptions_block.end());
    qSort(m_exceptions_allow.begin(), m_exceptions_allow.end());
    qSort(m_exceptions_allowForSession.begin(), m_exceptions_allowForSession.end());
}

void CookieJar::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("cookies"));
    QByteArray value = settings.value(QLatin1String("acceptCookies"),
                        QLatin1String("AcceptOnlyFromSitesNavigatedTo")).toByteArray();
    QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    m_acceptCookies = acceptPolicyEnum.keyToValue(value) == -1 ?
                        AcceptOnlyFromSitesNavigatedTo :
                        static_cast<AcceptPolicy>(acceptPolicyEnum.keyToValue(value));

    value = settings.value(QLatin1String("keepCookiesUntil"), QLatin1String("KeepUntilExpire")).toByteArray();
    QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
    m_keepCookies = keepPolicyEnum.keyToValue(value) == -1 ?
                        KeepUntilExpire :
                        static_cast<KeepPolicy>(keepPolicyEnum.keyToValue(value));

    m_sessionLength = settings.value(QLatin1String("sessionLength"), 0).toInt();
    m_filterTrackingCookies = settings.value(QLatin1String("filterTrackingCookies"), false).toBool();

    if (m_keepCookies == KeepUntilExit)
        setAllCookies(QList<QNetworkCookie>());

    emit cookiesChanged();
}

void CookieJar::save()
{
    if (!m_loaded || m_saved)
        return;

    QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    if (!QFile::exists(directory)) {
        QDir dir;
        dir.mkpath(directory);
    }

    saveCookies();

    // save cookie settings
    QSettings settings;
    settings.beginGroup(QLatin1String("cookies"));
    QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    settings.setValue(QLatin1String("acceptCookies"), QLatin1String(acceptPolicyEnum.valueToKey(m_acceptCookies)));

    QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
    settings.setValue(QLatin1String("keepCookiesUntil"), QLatin1String(keepPolicyEnum.valueToKey(m_keepCookies)));

    settings.setValue(QLatin1String("sessionLength"), m_sessionLength);
    settings.setValue(QLatin1String("filterTrackingCookies"), m_filterTrackingCookies);

    settings.sync();

    m_saved = true;
}

void CookieJar::saveCookies()
{
    purgeOldCookies();

    QList<QNetworkCookie> cookies = allCookies();
    for (int i = cookies.count() - 1; i >= 0; --i) {
        if (cookies.at(i).isSessionCookie())
            cookies.removeAt(i);
    }

    QFile f(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/cookies.data"));
    if (!f.open(QFile::WriteOnly))
        return;

    QDataStream s(&f);

    s << cookieJarMagic;
    s << cookieJarVersion;
    s << cookies;
    s << m_exceptions_block;
    s << m_exceptions_allow;
    s << m_exceptions_allowForSession;
}

void CookieJar::purgeOldCookies()
{
    QList<QNetworkCookie> cookies = allCookies();
    if (cookies.isEmpty())
        return;

    int oldCount = cookies.count();
    QDateTime now = QDateTime::currentDateTime();
    for (int i = cookies.count() - 1; i >= 0; --i) {
        if (!cookies.at(i).isSessionCookie() && cookies.at(i).expirationDate() < now)
            cookies.removeAt(i);
    }

    if (oldCount == cookies.count())
        return;

    setAllCookies(cookies);

    emit cookiesChanged();
}
