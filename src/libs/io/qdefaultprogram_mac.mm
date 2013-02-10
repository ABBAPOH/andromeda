#include "qdefaultprogram.h"
#include "qdefaultprogram_p.h"

#include <AppKit/AppKit.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

#include <QtGui/QFileIconProvider>

#include "qmimedatabase.h"

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

static QDefaultProgram progamInfo(CFBundleRef bundle)
{
    CFURLRef url = CFBundleCopyBundleURL(bundle);
    CFStringRef cfPath = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    CFRelease(url);
    QString path = getQString(cfPath);
    CFRelease(cfPath);

    CFStringRef identifier = CFBundleGetIdentifier(bundle);
    CFStringRef copyright = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleGetInfoString"));
    CFStringRef name = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleDisplayName"));
    CFStringRef version = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleShortVersionString"));
//    CFStringRef icon = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleIconFile"));

    QFileInfo info(path);

    QDefaultProgramData data;
    data.comment = QString();
    data.copyright = getQString(copyright);
    data.genericName = QString();
    data.path = info.absoluteFilePath();
    // we can't use QIcon because it doesn't supports .icns format
    data.icon = QFileIconProvider().icon(info);
//    data.icon = QIcon(data.path + "/Contents/Resources/" + getQString(icon));
    data.identifier = getQString(identifier);
    data.name = getQString(name);
    if (data.name.isEmpty())
        data.name = info.baseName();
    data.version = getQString(version);

    if (data.path.isEmpty())
        return QDefaultProgram();

    return QDefaultProgram(data);
}

// deprecated
#if 0
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
    CFRelease(path);

    if (!url)
        return QDefaultProgram();

    CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, url);
    CFRelease(url);
    if (!bundle)
        return QDefaultProgram();

    QDefaultProgram result = ::progamInfo(bundle);
    CFRelease(bundle);
    return result;
}
#endif

#ifndef NO_DEFAULT_PROGRAM
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

QDefaultProgram QDefaultProgram::defaultProgram(const QString &qtMimeType)
{
    // TODO: use LSCopyApplicationForMIMEType
    QMimeDatabase db;
    QString extension = db.mimeTypeForName(qtMimeType).preferredSuffix();
    return progamInfo(defaultProgramForExtension(extension));
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
#endif

QDefaultProgramList QDefaultProgram::defaultPrograms(const QUrl &qurl)
{
    QString qUrlString = qurl.toString();
    CFStringRef urlString = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                         (const UniChar*)qUrlString.constData(),
                                                         qUrlString.length());

    CFURLRef url = CFURLCreateWithString(kCFAllocatorDefault, urlString, 0);
    CFRelease(urlString);
    if (!url)
        return QDefaultProgramList();

    QDefaultProgramList result;

    // TODO: move to separate method?
    // get the default application
    CFURLRef defaultApp;
    LSGetApplicationForURL((CFURLRef)url, kLSRolesAll, NULL, &defaultApp);
    if (defaultApp) {
        CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, defaultApp);
        if (bundle) {

            QDefaultProgram program = ::progamInfo(bundle);
            if (program.isValid())
                result.append(program);

            CFRelease(bundle);
        }
        CFRelease(defaultApp);
    }

    CFArrayRef applications = LSCopyApplicationURLsForURL(url, kLSRolesAll);
    CFRelease(url);
    if (!applications)
        return result;

    CFIndex count = CFArrayGetCount(applications);

    for (CFIndex index = 0; index < count; index++) {
        CFURLRef url = (CFURLRef)CFArrayGetValueAtIndex(applications, index);

        CFBundleRef bundle = CFBundleCreate(kCFAllocatorDefault, url);
        if (!bundle)
            continue;

        QDefaultProgram program = ::progamInfo(bundle);
        if (program.isValid())
            result.append(program);

        CFRelease(bundle);
    }

    CFRelease(applications);

    // TODO: filter by name & version, but not path?
    QDefaultProgramPrivate::removeDuplicates(result);

    return result;
}

bool QDefaultProgram::openUrl(const QUrl &qurl) const
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
    result = FSPathMakeRef((const UInt8*)path().toUtf8().constData(), &fs, 0);
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

bool QDefaultProgram::openUrls(const QList<QUrl> &urls) const
{
    bool result = true;
    foreach (const QUrl &url, urls) {
        result &= openUrl(url);
    }
    return result;
}
