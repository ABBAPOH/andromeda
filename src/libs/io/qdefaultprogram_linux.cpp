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

static QString findDesktopFile(const QString &application)
{
    QStringList paths;
    paths.append(dataHome() + QLatin1String("/applications"));
    paths.append(QLatin1String("/usr/local/share/applications"));
    paths.append(QLatin1String("/usr/share/applications"));

    QString relativePath = application;
    relativePath.replace(QLatin1Char('-'), QLatin1Char('/'));
    foreach (const QString &path, paths) {
        QString filePath = path + QLatin1Char('/') + relativePath;
        if (QFile::exists(filePath))
            return filePath;
    }

    return QString();
}

static bool LessThan(const QString s1, const QString s2)
{
    int w1 = 0, w2 = 0;

    QString file;
    file = findDesktopFile(s1);
    if (!file.isEmpty()) {
        KDESettings s(file);
        s.beginGroup("Desktop Entry");
        w1 = s.value("InitialPreference", 5).toInt();
    }

    file = findDesktopFile(s2);
    if (!file.isEmpty()) {
        KDESettings s(file);
        s.beginGroup("Desktop Entry");
        w2 = s.value("InitialPreference", 5).toInt();
    }

    return w1 >= w2;
}

// TODO : speedup this hell
static QStringList sortByWeight(const QStringList & programs)
{
    QStringList result = programs;
    qSort(result.begin(), result.end(), LessThan);
    return result;
}

static QMap<QString, QStringList> getDefaultPrograms()
{
    QMap<QString, QStringList> result;
    QStringList folders;

    folders << "/usr/share/applications/" <<
               "/usr/local/share/applications/" <<
               dataHome() + QLatin1String("/applications");
    foreach (const QString &folder, folders) {
        QFileInfo info;

        info = QFileInfo(folder + "/" + "mimeinfo.cache");
        if (info.exists()) {
            KDESettings mimeCache(info.absoluteFilePath());
            mimeCache.beginGroup("MIME Cache");

            foreach (const QString &mimeType, mimeCache.allKeys()) {
                QStringList list = result.value(mimeType);

                list.append(mimeCache.value(mimeType).toStringList()); // join programs from all caches
                list.removeAll(QString(""));
                list.removeDuplicates();

                result.insert(mimeType, list);
            }
        }

        // read stupid gnome apps
        info = QFileInfo(folder + "/" + "defaults.list");
        if (info.exists()) {
            KDESettings defaultsList(info.absoluteFilePath());

            defaultsList.beginGroup("Default Applications");
            foreach (const QString &mimeType, defaultsList.allKeys()) {
                QStringList list = result.value(mimeType);

                list.append(defaultsList.value(mimeType).toStringList());
                list.removeAll(QString(""));
                list.removeDuplicates();

                result.insert(mimeType, list);
            }
            defaultsList.endGroup();
        }
    }

    foreach (const QString &key, result.keys()) {
        result.insert(key, sortByWeight(result.value(key)));
    }

    QFileInfo info(dataHome() + QLatin1String("/applications/") + "mimeapps.list");
    if (!info.exists())
        return result;

    KDESettings mimeApps(info.absoluteFilePath());
    mimeApps.beginGroup("Added Associations");

    foreach (const QString &mimeType, mimeApps.allKeys()) {
        QStringList list = mimeApps.value(mimeType).toStringList();
        list.removeAll(QString(""));
        result.insert(mimeType, list);
    }

    mimeApps.endGroup();

    mimeApps.beginGroup("Default Applications");

    foreach (const QString &mimeType, mimeApps.allKeys()) {
        QStringList list = mimeApps.value(mimeType).toStringList();
        list.removeAll(QString(""));
        result.insert(mimeType, list);
    }

    mimeApps.endGroup();

    mimeApps.beginGroup("Removed Associations");
    foreach (const QString &mimeType, mimeApps.allKeys()) {
        QStringList apps = result.value(mimeType);
        QStringList removedApps = mimeApps.value(mimeType).toStringList();
        foreach (const QString &app, removedApps) {
            apps.removeAll(app);
        }
        result.insert(mimeType, apps);
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

static QStringList defaultPrograms(const QString &mimeTypeName)
{
    QMimeDatabase db;

    QMap<QString, QStringList> programs = getDefaultPrograms();

    QStringList mimeTypes;
    QMimeType mimeType = db.mimeTypeForName(mimeTypeName);
    mimeTypes.append(mimeType.name());
    mimeTypes.append(mimeType.allAncestors());

    QStringList result;

    foreach (const QString &mimeType, mimeTypes) {
        result.append(programs.value(mimeType));
    }

    result.removeDuplicates();

    return result;
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

QDefaultProgram QDefaultProgram::progamInfo(const QString &application)
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

QString QDefaultProgram::defaultProgram(const QString &mimeType)
{
    QStringList programs = ::defaultPrograms(mimeType);
    if (programs.isEmpty())
        return QString();

    return programs.first();
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

QStringList QDefaultProgram::defaultPrograms(const QUrl &url)
{
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(url);
    return ::defaultPrograms(mimeType.name());
}

bool QDefaultProgram::openUrlWith(const QUrl &url, const QString &application)
{
    return openUrlsWith(QList<QUrl>() << url, application);
}

bool QDefaultProgram::openUrlsWith(const QList<QUrl> &urls, const QString &application)
{
    if (urls.isEmpty())
        return false;

    QString filePath = findDesktopFile(application);
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
