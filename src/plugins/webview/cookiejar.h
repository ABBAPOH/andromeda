#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QtNetwork/QNetworkCookieJar>

#include <QtCore/QStringList>

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT
    Q_DISABLE_COPY(CookieJar)

    Q_PROPERTY(AcceptPolicy acceptPolicy READ acceptPolicy WRITE setAcceptPolicy)
    Q_PROPERTY(QStringList allowedCookies READ allowedCookies WRITE setAllowedCookies)
    Q_PROPERTY(QStringList allowForSessionCookies READ allowForSessionCookies WRITE setAllowForSessionCookies)
    Q_PROPERTY(QStringList blockedCookies READ blockedCookies WRITE setBlockedCookies)
    Q_PROPERTY(KeepPolicy keepPolicy READ keepPolicy WRITE setKeepPolicy)
    Q_PROPERTY(int sessionLength READ sessionLength WRITE setSessionLength)
    Q_PROPERTY(bool filterTrackingCookies READ filterTrackingCookies WRITE setFilterTrackingCookies)

public:
    enum AcceptPolicy {
        AcceptAlways,
        AcceptNever,
        AcceptOnlyFromSitesNavigatedTo
    };
    Q_ENUMS(AcceptPolicy)

    enum KeepPolicy {
        KeepUntilExpire,
        KeepUntilExit,
        KeepUntilTimeLimit
    };
    Q_ENUMS(KeepPolicy)

    explicit CookieJar(QObject *parent = 0);
    ~CookieJar();

    AcceptPolicy acceptPolicy() const;
    void setAcceptPolicy(AcceptPolicy policy);

    QStringList allowedCookies() const;
    void setAllowedCookies(const QStringList &list);

    QStringList allowForSessionCookies() const;
    void setAllowForSessionCookies(const QStringList &list);

    QStringList blockedCookies() const;
    void setBlockedCookies(const QStringList &list);

    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

    KeepPolicy keepPolicy() const;
    void setKeepPolicy(KeepPolicy policy);

    int sessionLength() const;
    void setSessionLength(int sessionLength);

    bool filterTrackingCookies() const;
    void setFilterTrackingCookies(bool filter);

public slots:
    void clear();

protected:
    void setAllCookies(const QList<QNetworkCookie> & cookieList);

signals:
    void cookiesChanged();

private:
    inline void ensureLoaded();
    inline void ensureLoaded() const;
    void load();
    void loadCookies();
    void loadSettings();
    void save();
    void saveCookies();
    void purgeOldCookies();

private:
    bool m_loaded;
    bool m_saved;

    AcceptPolicy m_acceptCookies;
    KeepPolicy m_keepCookies;
    int m_sessionLength;
    bool m_filterTrackingCookies;

    QStringList m_exceptions_block;
    QStringList m_exceptions_allow;
    QStringList m_exceptions_allowForSession;

    friend class CookieModel;
};

#endif // COOKIEJAR_H

