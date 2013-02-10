#include "qdefaultprogram.h"
#include "qdefaultprogram_p.h"

#include <AppKit/AppKit.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#include <QtGui/QFileIconProvider>

#include "qmimedatabase.h"

static CFStringRef copyBundleUrl(CFStringRef bundle)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSString *string = [[NSWorkspace
            sharedWorkspace]
            absolutePathForAppBundleWithIdentifier:(NSString*)bundle];

    CFStringRef result = string ? CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)string) : 0;
    [pool release];
    return (CFStringRef)result;
}

static QString getQString(CFStringRef string)
{
    if (!string)
        return QString();

    CFIndex length = CFStringGetLength(string);

    QString result(length, QChar(0));
    CFRange range = {0, length};
    CFStringGetCharacters(string, range, (UniChar*)result.data());

    return result;
}

static QString defaultProgramForExtension(const QString &qtExtension)
{
    QString result;

    CFStringRef extension = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                        (const UniChar*)qtExtension.constData(),
                                                        qtExtension.length());
    if (!extension)
        return QString();

    CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, extension, NULL);
    CFRelease(extension);
    if (!uti)
        return QString();

    CFStringRef handler = LSCopyDefaultRoleHandlerForContentType(uti, kLSRolesAll);
    CFRelease(uti);
    if (!handler)
        return QString();

    result = getQString(handler).toLower();

    CFRelease(handler);

    return result;
}

static QString applicationPath(const QString &application)
{
    CFStringRef handler = CFStringCreateWithCString(kCFAllocatorDefault,
                                                    application.toUtf8().constData(),
                                                    kCFStringEncodingUTF8);
    CFStringRef appPath = copyBundleUrl(handler);
    if (!appPath) {
        CFRelease(handler);
        return QString();
    }

    QString result = getQString(appPath);
    CFRelease(appPath);
    CFRelease(handler);
    return result;
}

QDefaultProgram QDefaultProgram::progamInfo(const QString &application)
{
    CFStringRef id = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                  (const UniChar*)application.constData(),
                                                  application.length());
    CFStringRef path = copyBundleUrl(id);
    CFRelease(id);
    if (!path)
        return QDefaultProgram();

    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                 path,
                                                 kCFURLPOSIXPathStyle,
                                                 true);
    QFileInfo info(getQString(path));
    CFRelease(path);

    if (!url)
        return QDefaultProgram();

    CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, url);
    CFRelease(url);
    if (!bundle)
        return QDefaultProgram();

    CFStringRef copyright = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleGetInfoString"));
    CFStringRef name = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleDisplayName"));
    CFStringRef version = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleShortVersionString"));
//    CFStringRef icon = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleIconFile"));

    QDefaultProgramData data;
    data.comment = QString();
    data.copyright = getQString(copyright);
    data.genericName = QString();
    data.path = info.filePath();
    // we can't use QIcon because it doesn't supports .icns format
    data.icon = QFileIconProvider().icon(info);
//    data.icon = QIcon(data.path + "/Contents/Resources/" + getQString(icon));
    data.identifier = application;
    data.name = getQString(name);
    if (data.name.isEmpty())
        data.name = info.baseName();
    data.version = getQString(version);

    CFRelease(bundle);

    if (data.path.isEmpty())
        return QDefaultProgram();

    return QDefaultProgram(data);
}

QString QDefaultProgram::defaultProgram(const QString &qtMimeType)
{
    QMimeDatabase db;
    QString extension = db.mimeTypeForName(qtMimeType).preferredSuffix();
    return defaultProgramForExtension(extension);
}

bool QDefaultProgram::setDefaultProgram(const QString &qtMimeType, const QString &program)
{
    QMimeDatabase db;
    QString extension = db.mimeTypeForName(qtMimeType).preferredSuffix();

    CFStringRef suffix = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                      (const UniChar*)extension.constData(),
                                                      extension.length());
    if (!suffix)
        return false;

    CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, suffix, NULL);
    CFRelease(suffix);
    if (!uti)
        return false;

    CFStringRef id = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                  (const UniChar*)program.constData(),
                                                  program.length());
    if (!id) {
        CFRelease(uti);
        return false;
    }

    OSStatus result = LSSetDefaultRoleHandlerForContentType(uti, kLSRolesAll, id);

    CFRelease(uti);
    CFRelease(id);

    return result == noErr;
}

QStringList QDefaultProgram::defaultPrograms(const QUrl &qurl)
{
    QStringList result;

    QString defaultProgram = QDefaultProgram::defaultProgram(qurl);
    if (!defaultProgram.isEmpty())
        result.append(defaultProgram);

    QString qUrlString = qurl.toString();
    CFStringRef urlString = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                         (const UniChar*)qUrlString.constData(),
                                                         qUrlString.length());

    CFURLRef url = CFURLCreateWithString(kCFAllocatorDefault, urlString, 0);
    CFRelease(urlString);
    if (!url)
        return result;

    CFArrayRef applications = LSCopyApplicationURLsForURL(url, kLSRolesAll);
    CFRelease(url);
    if (!applications)
        return result;

    CFIndex count = CFArrayGetCount(applications);

    for (CFIndex index = 0; index < count; index++) {
        CFURLRef url = (CFURLRef)CFArrayGetValueAtIndex(applications, index);
        CFShow(url);

        CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, url);
        if (!bundle)
            continue;

        CFStringRef bundleId = CFBundleGetIdentifier(bundle);
        if (bundleId) {
            result.append(getQString(bundleId).toLower());
        }

        CFRelease(bundle);
    }

    CFRelease(applications);

    result.removeDuplicates();

    return result;
}

bool QDefaultProgram::openUrlWith(const QUrl &qurl, const QString &application)
{
    QString qUrlString = qurl.toString();

    CFStringRef urlString = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                         (const UniChar*)qUrlString.constData(),
                                                         qUrlString.length());

    CFURLRef url = CFURLCreateWithString(kCFAllocatorDefault, urlString, 0);
    CFRelease(urlString);
    if (!url)
        return false;

    const void *urlsData[] = { url };
    CFArrayRef urls = CFArrayCreate(kCFAllocatorDefault, urlsData, 1, 0);
    if (!urls) {
        CFRelease(url);
        return false;
    }

    OSStatus result;
    FSRef fs;
    result = FSPathMakeRef((const UInt8*)applicationPath(application).toUtf8().constData(), &fs, 0);
    if (result != noErr) {
        CFRelease(urls);
        CFRelease(url);
        return false;
    }

    LSApplicationParameters appParams;
    appParams.version = 0;
    appParams.flags = kLSLaunchDefaults;
    appParams.application = &fs;
    appParams.asyncLaunchRefCon = 0;
    appParams.environment = 0;
    appParams.argv = 0;
    appParams.initialEvent = 0;
    result = LSOpenURLsWithRole(urls, kLSRolesAll, 0, &appParams, 0, 0);

    CFRelease(urls);
    CFRelease(url);

    return result == noErr;
}

bool QDefaultProgram::openUrlsWith(const QList<QUrl> &urls, const QString &application)
{
    bool result = true;
    foreach (const QUrl &url, urls) {
        result &= openUrlWith(url, application);
    }
    return result;
}
