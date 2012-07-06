#include "qdefaultprogram.h"
#include "qdefaultprogram_p.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtGui/QFileIconProvider>

#include "qmimetype.h"
#include "qmimedatabase.h"

static QString replaceVariables(const QString &string)
{
    QString result;
    QRegExp rx("%.*%");
    rx.setMinimal(true);

    int pos = 0;
    int oldPos = 0;
    while ((pos = rx.indexIn(string, pos)) != -1) {
        result.append(string.mid(oldPos, pos - oldPos));
        QString var = string.mid(pos + 1, rx.matchedLength() - 2);
        QString val = QString::fromLocal8Bit(qgetenv(var.toLocal8Bit().constData()));
        // In case if variable has other variables in it, we replace them too.
        result.append(replaceVariables(val));
        pos += rx.matchedLength();
        oldPos = pos;
    }
    result.append(string.mid(oldPos));
    return result;
}

static QString getAppOpenCommand(const QString &appId)
{
    QSettings reg(QString("HKEY_CLASSES_ROOT\\Applications\\%1").arg(appId), QSettings::NativeFormat);

    QString command;
    reg.beginGroup("shell/open/command");
    command = reg.value(".").toString();
    reg.endGroup();

    if (command.isEmpty()) {
        reg.beginGroup("shell/edit/command");
        command = reg.value(".").toString();
        reg.endGroup();
    }

    return command;
}

// Split string in a form of ""app.exe" "file.bin""
static QStringList splitCommand(const QString &command)
{
    QStringList arguments;

    arguments = command.split( QRegExp(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)") );
    for (int i = 0; i < arguments.count(); i++) {
        QString argument = arguments[i];
        if (argument.startsWith(QLatin1Char('"')) && argument.endsWith(QLatin1Char('"')))
            argument = argument.mid(1, argument.length() - 2);
        arguments[i] = argument;
    }

    return arguments;
}

static QString getAppFromProgId(const QString &progId)
{
    QSettings reg(QString("HKEY_CLASSES_ROOT\\%1\\shell\\open\\command").arg(progId), QSettings::NativeFormat);

    QString command = reg.value(".").toString();
    if (command.isEmpty())
        return QString();

    QStringList args = splitCommand(command);
    if (args.isEmpty())
        return QString();

    QString appPath = args.first();
    // hack for rundll32.exe - we can't get dll name easily
    if (appPath == "rundll32.exe")
        return QString();
    // TODO: check if key esists in Applications
    return QFileInfo(replaceVariables(appPath)).fileName();
}

static QStringList getSystemFileAssociationsDefaultPrograms(const QString &type)
{
    QSettings reg("HKEY_CLASSES_ROOT\\SystemFileAssociations", QSettings::NativeFormat);

    reg.beginGroup(type);
    reg.beginGroup("OpenWithList");

    return reg.childGroups();
}

static QStringList getDefaultPrograms(const QString &scope, const QString &group, const QString &extension)
{
    QStringList result;

    QSettings reg(QString("%1\\%2").arg(scope).arg(group), QSettings::NativeFormat);

    reg.beginGroup(extension);

    reg.beginGroup("OpenWithList");

    QStringList childGroups = reg.childGroups();
    if (childGroups.isEmpty()) {
        QString list = reg.value("MRUList").toString();
        for (int i = 0; i < list.count(); i++) {
            result.append(reg.value(list.at(i)).toString());
        }
    } else {
        result.append(childGroups);
    }

    reg.endGroup();

    reg.beginGroup("OpenWithProgids");

    foreach (const QString &progId, reg.childKeys()) {
        if (progId == "." || progId == "Default")
            continue;
        QString app = getAppFromProgId(progId);
        if (!app.isEmpty())
            result.append(app);
    }

    reg.endGroup();

    QString contentType = reg.value("PerceivedType").toString();
    if (!contentType.isEmpty()) {
        result.append(getSystemFileAssociationsDefaultPrograms(contentType));
    }

    return result;
}

static QStringList getExtensionDefaultPrograms(const QString &extension)
{
    QStringList result;
    result.append(getDefaultPrograms("HKEY_CURENT_USER", "SOFTWARE\\Classes", extension));
    result.append(getDefaultPrograms("HKEY_LOCAL_MACHINE", "SOFTWARE\\Classes", extension));
    return result;
}

static QStringList getExplorerDefaultPrograms(const QString &extension)
{
    QStringList result;
    result.append(getDefaultPrograms("HKEY_CURRENT_USER", "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts", extension));
    result.append(getDefaultPrograms("HKEY_LOCAL_MACHINE", "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts", extension));
    return result;
}

static QStringList removeDuplicates(const QStringList &list)
{
    QStringList result;
    foreach (const QString &string, list) {
        if (!result.contains(string, Qt::CaseInsensitive))
            result.append(string.toLower());
    }

    return result;
}

static QString getDefaultProgram(const QString &extension)
{
    QSettings reg(QString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts"), QSettings::NativeFormat);

    reg.beginGroup(extension);
    return reg.value("Application").toString();
}

static bool setDefaultProgram(const QString &extension, const QString &program)
{
    QSettings reg(QString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts"), QSettings::NativeFormat);

    reg.beginGroup(extension);
    reg.setValue("Application", program);

    return reg.status() == QSettings::NoError;
}

static QString whereis(const QString &binaryName)
{
    QFileInfo info(binaryName);
    if (info.isAbsolute())
        return binaryName;

    QByteArray path = qgetenv("PATH");
    QList<QByteArray> paths = path.split(';');

    foreach (const QByteArray &path, paths) {
        QString absolutePath = QDir::fromNativeSeparators(QString::fromLocal8Bit(path)) + "/" + binaryName;
        if (QFile::exists(absolutePath))
            return absolutePath;
    }

    return QString();
}

static QString getAppPath(const QString &scope, const QString &program)
{
    // TODO: use "shell/open" to detect path
    QSettings reg(QString("%1\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths").arg(scope), QSettings::NativeFormat);

    reg.beginGroup(program);
    QString path = replaceVariables(reg.value(".").toString()).replace(QLatin1String("\""), QString());
    if (path.isEmpty()) {
        path = whereis(program);
    }

    return path;
}

static QString getAppPath(const QString &application)
{
    QString path = getAppPath("HKEY_CURRENT_USER", application);
    if (path.isEmpty())
        path = getAppPath("HKEY_LOCAL_MACHINE", application);

    if (path.isEmpty()) {
        QString command = getAppOpenCommand(application);

        QStringList arguments = splitCommand(command);
        if (!arguments.isEmpty())
            path = replaceVariables(arguments.first());
    }

    return path;
}

static QString getAppName(const QString &path)
{
    // TODO: qtbug?
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache", QSettings::NativeFormat);

    QString name = reg.value(path).toString();
    if (name.isEmpty())
        name = QFileInfo(path).baseName();
    return name;
}

static QString urlToString(const QUrl &url)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
    if (url.isLocalFile())
#else
    if (url.scheme() == QLatin1String("file"))
#endif
        return QDir::toNativeSeparators(url.toLocalFile());

    return url.toString();
}

QDefaultProgram QDefaultProgram::progamInfo(const QString &application)
{
    QDefaultProgramData data;

    QString path = getAppPath(application);
    if (path.isEmpty())
        return QDefaultProgram();

    QFileInfo info(path);
    if (!info.exists())
        return QDefaultProgram();

    data.path = info.canonicalFilePath();
    data.icon = QFileIconProvider().icon(info);
    data.identifier = application.toLower();
    data.name = getAppName(data.path);

    return QDefaultProgram(data);
}

QString QDefaultProgram::defaultProgram(const QString &mimeType)
{
    QMimeDatabase db;

    QString extension = QLatin1String(".") + db.mimeTypeForName(mimeType).preferredSuffix();
    if (extension == QLatin1String("."))
        return QString();

    QString result = getDefaultProgram(extension);
    if (!result.isEmpty())
        return result;

    // TODO: try detect from extension in HCLR?

    QStringList list = defaultPrograms(mimeType);
    if (!list.isEmpty())
        return list.first();

    return QString();
}

bool QDefaultProgram::setDefaultProgram(const QString &mimeType, const QString &program)
{
    QMimeDatabase db;

    QString extension = QLatin1String(".") + db.mimeTypeForName(mimeType).preferredSuffix();
    if (extension == QLatin1String("."))
        return false;

    return ::setDefaultProgram(extension, program);
}

QStringList QDefaultProgram::defaultPrograms(const QUrl &url)
{
    QFileInfo info(url.path());
    QString extension = QLatin1Char('.') + info.suffix();

    QStringList result;
    result.append(getExplorerDefaultPrograms(extension));
    result.append(getExtensionDefaultPrograms(extension));
    result.removeDuplicates();

    return removeDuplicates(result);
}

bool QDefaultProgram::openUrlWith(const QUrl &url, const QString &application)
{
    QString command = getAppOpenCommand(application);
    QStringList arguments = splitCommand(command);
    for (int i = 0; i < arguments.count(); i++) {
        if (arguments[i] == QLatin1String("%1"))
            arguments[i] = urlToString(url);

        arguments[i] = replaceVariables(arguments[i]);
    }

    if (arguments.isEmpty())
        return false;

    // hack for rundll32.exe - we need to pass single string as an argument
    if (arguments.first() == "rundll32.exe")
        return QProcess::startDetached(arguments.join(" "));

    return QProcess::startDetached(arguments.first(), arguments.mid(1));
}

bool QDefaultProgram::openUrlsWith(const QList<QUrl> &urls, const QString &application)
{
    bool result = true;
    foreach (const QUrl &url, urls) {
        result &= openUrlWith(url, application);
    }
    return result;
}
