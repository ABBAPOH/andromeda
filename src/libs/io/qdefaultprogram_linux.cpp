#include "qdefaultprogram.h"
#include "qdefaultprogram_p.h"

#include <QDir>
#include <QDirIterator>
#include <QMap>
#include <QSettings>
#include <QTemporaryFile>
#include <QProcess>

#include "kdesettings.h"
#include "qmimedatabase.h"
#include "qxdg.h"

//#include <QDebug>

struct DesktopEntry
{
    DesktopEntry() {}

    bool read(const QString &application);
    QDefaultProgram toDefaultProgram() const;

    QString comment;
    QString copyright;
    QString genericName;
    QIcon icon;
    QString identifier;
    QString name;
    QString path;
    QString version;

    int weight;
};

typedef QList<DesktopEntry *> DesktopEntryList;

static QString findDesktopFile(const QString &application)
{
    QStringList paths;
    paths.append(dataHome() + QLatin1String("/applications"));
    paths.append(QLatin1String("/usr/local/share/applications"));
    paths.append(QLatin1String("/usr/share/applications"));

    QStringList relativePaths;
    relativePaths.append(application);
    relativePaths.append(QString(application).replace(QLatin1Char('-'), QLatin1Char('/')));
    foreach (const QString &path, paths) {
        foreach (const QString &relativePath, relativePaths) {
            QString filePath = path + QLatin1Char('/') + relativePath;
            if (QFile::exists(filePath))
                return filePath;
        }
    }

    return QString();
}

static QStringList getApplicationFolders()
{
    return { "/usr/share/applications/",
            "/usr/local/share/applications/",
            dataHome() + "/applications" };
}

static DesktopEntry *getEntry(QHash<QString, DesktopEntry *> &entryMap, const QString &app)
{
    if (app.isEmpty())
        return 0;

    DesktopEntry *entry = entryMap.value(app);
    if (!entry) {
        entry = new DesktopEntry;
        if (entry->read(app)) {
            entryMap.insert(app, entry);
        } else {
            delete entry;
            entry = 0;
        }
    }
    return entry;
}

static void readCacheFile(QHash<QString, DesktopEntry *> &entryMap,
                          QHash<QString, DesktopEntryList> &mimeTypeMap,
                          const QStringList &mimeTypes,
                          const QString &file,
                          const QString &defaultGroup)
{
    QFileInfo info(file);

    if (!info.exists())
        return;

    KDESettings mimeCache(info.absoluteFilePath());
    mimeCache.beginGroup(defaultGroup);

    foreach (const QString &mimeType, mimeTypes) {
        DesktopEntryList list = mimeTypeMap.value(mimeType);

        QStringList apps = mimeCache.value(mimeType).toStringList();

        foreach (const QString &app, apps) {
            DesktopEntry *entry = getEntry(entryMap, app);
            if (!entry)
                continue;

            if (!list.contains(entry))
                list.append(entry);
        }

        mimeTypeMap.insert(mimeType, list);
    }
}

static void readMimeAppsFile(QHash<QString, DesktopEntry *> &entryMap,
                             QHash<QString, DesktopEntryList> &mimeTypeMap,
                             const QStringList &mimeTypes,
                             const QString &file)
{
    QFileInfo info(file);
    if (!info.exists())
        return;

    KDESettings mimeApps(info.absoluteFilePath());
    // should be prepended to sorted list
    mimeApps.beginGroup("Added Associations");

    foreach (const QString &mimeType, mimeTypes) {
        DesktopEntryList list = mimeTypeMap.value(mimeType);

        QStringList apps = mimeApps.value(mimeType).toStringList();

        DesktopEntryList newList;

        foreach (const QString &app, apps) {
            DesktopEntry *entry = getEntry(entryMap, app);
            if (!entry)
                continue;

            list.removeOne(entry);
            newList.append(entry);
        }

        newList.append(list);
        mimeTypeMap.insert(mimeType, newList);
    }

    mimeApps.endGroup();

    // entries should be removed from mime type map
    mimeApps.beginGroup("Removed Associations");
    foreach (const QString &mimeType, mimeTypes) {
        DesktopEntryList list = mimeTypeMap.value(mimeType);

        QStringList apps = mimeApps.value(mimeType).toStringList();

        foreach (const QString &app, apps) {
            DesktopEntry *entry = getEntry(entryMap, app);
            if (!entry)
                continue;

            list.removeOne(entry);
        }

        mimeTypeMap.insert(mimeType, list);
    }

}

static QList<QDefaultProgram> programs(const QStringList &mimeTypes)
{
    QHash<QString, DesktopEntry *> entryMap;
    QHash<QString, DesktopEntryList> mimeTypeMap;

    QStringList folders = getApplicationFolders();

    foreach (const QString &folder, folders) {
        readCacheFile(entryMap,
                      mimeTypeMap,
                      mimeTypes,
                      folder + "/" + "mimeinfo.cache",
                      "MIME Cache");

        readCacheFile(entryMap,
                      mimeTypeMap,
                      mimeTypes,
                      folder + "/" + "defaults.list",
                      "Default Applications");
    }

    // now we should sort map using weight
    foreach (const QString &key, mimeTypeMap.keys()) {
        DesktopEntryList &entries = mimeTypeMap[key];
        auto lessThan = [](DesktopEntry *a, DesktopEntry *b) {
            return a->weight >= b->weight;
        };
        qSort(entries.begin(), entries.end(), lessThan);
    }

    const QString userAppsFile = dataHome() + QLatin1String("/applications/") + "mimeapps.list";
    readMimeAppsFile(entryMap, mimeTypeMap, mimeTypes, userAppsFile);

    QList<QDefaultProgram> result;
    foreach (const QString &mimeType, mimeTypes) {
        foreach (DesktopEntry *entry, mimeTypeMap.value(mimeType)) {
            result.append(entry->toDefaultProgram());
        }
    }
    return result;
}

///************************************************
// LC_MESSAGES value	Possible keys in order of matching
// lang_COUNTRY@MODIFIER	lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
//                        default value
// lang_COUNTRY	        lang_COUNTRY, lang, default value
// lang@MODIFIER	        lang@MODIFIER, lang, default value
// lang	                lang, default value
// ************************************************/
static QVariant localizedValue(QSettings &desktopFile,
                               const QString &key,
                               const QVariant &defaultValue = QVariant())
{
    QString lang = getenv("LC_MESSAGES");

    if (lang.isEmpty())
        lang = qgetenv("LC_ALL");

    if (lang.isEmpty())
         lang = qgetenv("LANG");

    QString modifier = lang.section('@', 1);
    if (!modifier.isEmpty())
        lang.truncate(lang.length() - modifier.length() - 1);

    QString encoding = lang.section('.', 1);
    if (!encoding.isEmpty())
        lang.truncate(lang.length() - encoding.length() - 1);


    QString country = lang.section('_', 1);
    if (!country.isEmpty())
        lang.truncate(lang.length() - country.length() - 1);


    //qDebug() << "LC_MESSAGES: " << getenv("LC_MESSAGES");
    //qDebug() << "Lang:" << lang;
    //qDebug() << "Country:" << country;
    //qDebug() << "Encoding:" << encoding;
    //qDebug() << "Modifier:" << modifier;

    if (!modifier.isEmpty() && !country.isEmpty()) {
        QString k = QString("%1[%2_%3@%4]").arg(key, lang, country, modifier);
//        qDebug() << "\t try " << k << desktopFile.contains(k);
        if (desktopFile.contains(k))
            return desktopFile.value(k, defaultValue);
    }

    if (!country.isEmpty()) {
        QString k = QString("%1[%2_%3]").arg(key, lang, country);
//        qDebug() << "\t try " << k  << desktopFile.contains(k);
        if (desktopFile.contains(k))
            return desktopFile.value(k, defaultValue);
    }

    if (!modifier.isEmpty()) {
        QString k = QString("%1[%2@%3]").arg(key, lang, modifier);
//        qDebug() << "\t try " << k  << desktopFile.contains(k);
        if (desktopFile.contains(k))
            return desktopFile.value(k, defaultValue);
    }

    QString k = QString("%1[%2]").arg(key, lang);
//    qDebug() << "\t try " << k  << desktopFile.contains(k);
    if (desktopFile.contains(k))
        return desktopFile.value(k, defaultValue);

//    qDebug() << "\t try " << key  << desktopFile.contains(key);
    return desktopFile.value(key, defaultValue);
}

static QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

static QStringList urlsToLocalFiles(const QList<QUrl> &urls)
{
    QStringList result;
    foreach (const QUrl &url, urls) {
        result.append(url.toLocalFile());
    }
    return result;
}

static QString urlToString(const QUrl &url)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
    if (url.isLocalFile())
#else
    if (url.scheme() == QLatin1String("file"))
#endif
        return url.toLocalFile();

    return url.toString();
}

static QStringList urlsToStrings(const QList<QUrl> &urls)
{
    QStringList result;

    foreach (const QUrl &url, urls) {
        result.append(urlToString(url));
    }

    return result;
}

static QStringList expandExecString(QSettings &desktopFile, const QList<QUrl> &urls)
{
    QStringList result;
    QStringList tokens = parseCombinedArgString(desktopFile.value("Exec").toString());
    foreach (const QString &token, tokens) {
        // ----------------------------------------------------------
        // A single file name, even if multiple files are selected.
        if (token == "%f")
        {
            if (!urls.isEmpty())
                result << urls.at(0).toLocalFile();
            continue;
        }

        // ----------------------------------------------------------
        // A list of files. Use for apps that can open several local files at once.
        // Each file is passed as a separate argument to the executable program.
        if (token == "%F") {
            result << urlsToLocalFiles(urls);
            continue;
        }

        // ----------------------------------------------------------
        // A single URL. Local files may either be passed as file: URLs or as file path.
        if (token == "%u") {
            if (!urls.isEmpty()) {
                result << urlToString(urls.at(0));
            }
            continue;
        }

        // ----------------------------------------------------------
        // A list of URLs. Each URL is passed as a separate argument to the executable
        // program. Local files may either be passed as file: URLs or as file path.
        if (token == "%U") {
            result << urlsToStrings(urls);
            continue;
        }

        // ----------------------------------------------------------
        // The Icon key of the desktop entry expanded as two arguments, first --icon
        // and then the value of the Icon key. Should not expand to any arguments if
        // the Icon key is empty or missing.
        if (token == "%i") {
            QString icon = desktopFile.value("Icon").toString();
            if (!icon.isEmpty())
                result << "-icon" << icon.replace('%', "%%");
            continue;
        }


        // ----------------------------------------------------------
        // The translated name of the application as listed in the appropriate Name key
        // in the desktop entry.
        if (token == "%c")
        {
            result << localizedValue(desktopFile, "Name").toString().replace('%', "%%");
            continue;
        }

        // ----------------------------------------------------------
        // The location of the desktop file as either a URI (if for example gotten from
        // the vfolder system) or a local filename or empty if no location is known.
        if (token == "%k") {
            result << desktopFile.fileName().replace('%', "%%");
            break;
        }

        // ----------------------------------------------------------
        // Deprecated.
        // Deprecated field codes should be removed from the command line and ignored.
        if (token == "%d" || token == "%D" ||
            token == "%n" || token == "%N" ||
            token == "%v" || token == "%m") {
            continue;
        }

        // ----------------------------------------------------------
        result << token;
    }

    return result;
}

static QList<QDefaultProgram> programs(const QMimeType &mimeType)
{
    QStringList mimeTypes;

    mimeTypes.append(mimeType.name());
    // TODO: correctly add aliases
//    mimeTypes.append(mimeType.aliases());
    mimeTypes.append(mimeType.allAncestors());

    return programs(mimeTypes);
}

static QString whereis(const QString &binaryName)
{
    QFileInfo info(binaryName);
    if (info.isAbsolute())
        return binaryName;

    QByteArray path = qgetenv("PATH");
    QList<QByteArray> paths = path.split(':');

    foreach (const QByteArray &path, paths) {
        QString absolutePath = QString::fromLocal8Bit(path) + "/" + binaryName;
        if (QFile::exists(absolutePath))
            return absolutePath;
    }

    return QString();
}

// deprecated
#if 0
static QDefaultProgram progamInfo(const QString &application)
{
    QString desktopFilePath = findDesktopFile(application);

    if (desktopFilePath.isEmpty())
        return QDefaultProgram();

    KDESettings desktopFile(desktopFilePath);
    desktopFile.beginGroup("Desktop Entry");

    if (desktopFile.status() != QSettings::NoError)
        return QDefaultProgram();

    QDefaultProgramData data;

    QStringList args = parseCombinedArgString(desktopFile.value("Exec").toString());
    QString binaryName = !args.isEmpty() ? args.first() : QString();

    data.comment = localizedValue(desktopFile, "Comment").toString();
    data.copyright = QString();
    data.genericName = localizedValue(desktopFile, "GenericName").toString();
    data.icon = QIcon::fromTheme(desktopFile.value("Icon").toString(), QIcon());
    data.identifier = application;
    data.name = localizedValue(desktopFile, "Name").toString();
    data.path = whereis(binaryName);
    data.version = desktopFile.value("Version").toString();

    return QDefaultProgram(data);
}
#endif

bool DesktopEntry::read(const QString &application)
{
    QString desktopFilePath = findDesktopFile(application);

    if (desktopFilePath.isEmpty())
        return false;

    KDESettings desktopFile(desktopFilePath);
    desktopFile.beginGroup("Desktop Entry");

    if (desktopFile.status() != QSettings::NoError)
        return false;

    DesktopEntry &data = *this;

    QStringList args = parseCombinedArgString(desktopFile.value("Exec").toString());
    QString binaryName = !args.isEmpty() ? args.first() : QString();

    data.comment = localizedValue(desktopFile, "Comment").toString();
    data.copyright = QString();
    data.genericName = localizedValue(desktopFile, "GenericName").toString();
    data.icon = QIcon::fromTheme(desktopFile.value("Icon").toString(), QIcon());
    data.identifier = application;
    data.name = localizedValue(desktopFile, "Name").toString();
    data.path = whereis(binaryName);
    data.version = desktopFile.value("Version").toString();
    data.weight = desktopFile.value("InitialPreference", 5).toInt();

    return true;
}

QDefaultProgram DesktopEntry::toDefaultProgram() const
{
    QDefaultProgramData data;

    data.comment = comment;
    data.copyright = copyright;
    data.genericName = genericName;
    data.icon = icon;
    data.identifier = identifier;
    data.name = name;
    data.path = path;
    data.version = version;

    return QDefaultProgram(data);
}

#ifndef NO_DEFAULT_PROGRAM
QDefaultProgram QDefaultProgram::defaultProgram(const QString &mimeType)
{
    QStringList programs = ::defaultPrograms(mimeType);
    if (programs.isEmpty())
        return QDefaultProgram();

    return progamInfo(programs.first());
}

bool QDefaultProgram::setDefaultProgram(const QString &mimeType, const QString &program)
{
    QFileInfo info(dataHome() + QLatin1String("/applications/") + "mimeapps.list");

    KDESettings mimeApps(info.absoluteFilePath());

    mimeApps.beginGroup("Added Associations");

    QStringList applications = mimeApps.value(mimeType).toStringList();
    applications.removeAll(program);
    applications.prepend(program);
    mimeApps.setValue(mimeType, applications);

    mimeApps.endGroup();

    mimeApps.beginGroup("Removed Associations");

    QStringList removedApps = mimeApps.value(mimeType).toStringList();
    removedApps.removeAll(program);
    if (!removedApps.isEmpty())
        mimeApps.setValue(mimeType, removedApps);
    else
        mimeApps.remove(mimeType);

    mimeApps.sync();

    return mimeApps.status() == QSettings::NoError;
}
#endif

QDefaultProgramList QDefaultProgram::defaultPrograms(const QUrl &url)
{
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(url);

    return programs(mimeType);
}

bool QDefaultProgram::openUrl(const QUrl &url) const
{
    return openUrls(QList<QUrl>() << url);
}

bool QDefaultProgram::openUrls(const QList<QUrl> &urls) const
{
    if (urls.isEmpty())
        return false;

    QString filePath = path();
    if (filePath.isEmpty())
        return false;

    KDESettings desktopFile(QFileInfo(filePath).absoluteFilePath());
    desktopFile.beginGroup("Desktop Entry");

    QStringList args = expandExecString(desktopFile, urls);

    if (args.isEmpty())
        return false;

    if (desktopFile.value("Terminal").toBool()) {
        QString term = getenv("TERM");
        if (term.isEmpty())
            term = "xterm";

        args.prepend("-e");
        args.prepend(term);
    }

    QString cmd = args.takeFirst();
    return QProcess::startDetached(cmd, args);
}
