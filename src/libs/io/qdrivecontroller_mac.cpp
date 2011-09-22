#include "qdrivecontroller.h"
#include "qdrivecontroller_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QThread>

#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CFRunLoop.h>

#include <DiskArbitration/DiskArbitration.h>

#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>

#include <QChar>

#include "qdriveinfo.h"

#define TIME_INTERVAL 1

class QDriveWatcherEngine : public QThread
{
    Q_OBJECT

public:
    QDriveWatcherEngine();
    ~QDriveWatcherEngine();

    void stop();

    void addDrive(const QString &path);
    void removeDrive(const QString &path);
    void updateDrives();

protected:
    void run();

Q_SIGNALS:
    void driveAdded(const QString &path);
    void driveRemoved(const QString &path);

private:
    void populateVolumes();

    volatile bool m_running;

    DASessionRef m_session;
    QSet<QString> volumes;
};

QString CFStringToQString(CFStringRef string)
{
    CFRange range = { 0, CFStringGetLength(string) };

    unsigned short *array = new unsigned short[range.length];
    CFStringGetCharacters(string, range, array);

    // TODO: remove copying and use QString::data()?
    QString result = QString::fromUtf16(array, range.length);

    delete [] array;
    return result;
}

QString getDiskPath(DADiskRef diskRef)
{
    CFDictionaryRef dictionary = DADiskCopyDescription(diskRef);
    if (!dictionary)
        return QString();

    CFURLRef url = (CFURLRef)CFDictionaryGetValue(dictionary, kDADiskDescriptionVolumePathKey);
    if (!url) {
        CFRelease(dictionary);
        return QString();
    }

    CFStringRef stringRef = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if (!stringRef) {
        CFRelease(dictionary);
        return QString();
    }

    QString path = CFStringToQString(stringRef);

    CFRelease(stringRef);
    CFRelease(dictionary);

    return path;
}

void checkNewDiskAndEmitSignal(DADiskRef disk, void *context)
{
    QString path = getDiskPath(disk);
    if (path.isEmpty())
        return;

    QDriveWatcherEngine *watcher = reinterpret_cast<QDriveWatcherEngine*>(context);
    watcher->addDrive(path);
}

// FIXME: here we get event when flash drive mounted
void mountCallback1(DADiskRef disk, CFArrayRef /*keys*/, void *context)
{
    checkNewDiskAndEmitSignal(disk, context);
}

// FIXME: here we get event when cdrom mounted? O_o
void mountCallback2(DADiskRef disk, void *context)
{
    checkNewDiskAndEmitSignal(disk, context);
}

void unmountCallback(DADiskRef disk, void *context)
{
//    CFShow(DADiskCopyDescription(disk));
    QString path = getDiskPath(disk);
    QDriveWatcherEngine *watcher = reinterpret_cast<QDriveWatcherEngine*>(context);
    if (!path.isEmpty()) {
        watcher->removeDrive(path);
    } else {
        // if we didn't receive path from API, we need to determine the lost drive manually
        watcher->updateDrives();
    }
}

DADissenterRef unmountCallback2(DADiskRef disk, void *context)
{
    unmountCallback(disk, context);
    return 0;
}

QDriveWatcherEngine::QDriveWatcherEngine() :
    QThread(),
    m_running(false)
{
    m_session = DASessionCreate(kCFAllocatorDefault);

    DARegisterDiskDescriptionChangedCallback(m_session,
                                             kDADiskDescriptionMatchVolumeMountable,
                                             kDADiskDescriptionWatchVolumePath,
                                             mountCallback1,
                                             this);
    DARegisterDiskAppearedCallback(m_session,
                                   kDADiskDescriptionMatchVolumeMountable,
                                   mountCallback2,
                                   this);
    DARegisterDiskDisappearedCallback(m_session,
                                      kDADiskDescriptionMatchVolumeMountable,
                                      unmountCallback,
                                      this); // to catch event about ejecting
    DARegisterDiskUnmountApprovalCallback(m_session,
                                          kDADiskDescriptionMatchVolumeMountable,
                                          unmountCallback2,
                                          this); // to catch event about unmounting and ejecting
    start();
}

QDriveWatcherEngine::~QDriveWatcherEngine()
{
    stop();

    DAUnregisterCallback(m_session, (void*)mountCallback1, this);
    DAUnregisterCallback(m_session, (void*)mountCallback2, this);
    DAUnregisterCallback(m_session, (void*)unmountCallback, this);

    CFRelease(m_session);
}

void QDriveWatcherEngine::stop()
{
    m_running = false;
    wait();
}

void QDriveWatcherEngine::run()
{
    m_running = true;

    populateVolumes();

    DASessionScheduleWithRunLoop(m_session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

    SInt32 result;
    do {
        result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, TIME_INTERVAL, true);
    } while (m_running && result);

    DASessionUnscheduleFromRunLoop(m_session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
}

void QDriveWatcherEngine::populateVolumes()
{
    // TODO: get paths as StringList?
    foreach (const QDriveInfo &info, QDriveInfo::drives())
        volumes.insert(info.rootPath());
}

void QDriveWatcherEngine::addDrive(const QString &path)
{
    if (!volumes.contains(path)) {
        volumes.insert(path);
        emit driveAdded(path);
    }
}

void QDriveWatcherEngine::removeDrive(const QString &path)
{
    if (volumes.remove(path))
        emit driveRemoved(path);
}

void QDriveWatcherEngine::updateDrives()
{
    QSet<QString> oldDrives = volumes;

    volumes.clear();
    populateVolumes();

    foreach (const QString &path, oldDrives) {
        if (!volumes.contains(path))
            emit driveRemoved(path);
    }

    foreach (const QString &path, volumes) {
        if (!oldDrives.contains(path))
            emit driveAdded(path);
    }
}

bool QDriveWatcher::start_sys()
{
    engine = new QDriveWatcherEngine;
    connect(engine, SIGNAL(driveAdded(QString)), this, SIGNAL(driveAdded(QString)));
    connect(engine, SIGNAL(driveRemoved(QString)), this, SIGNAL(driveRemoved(QString)));
    return true;
}

void QDriveWatcher::stop_sys()
{
    delete engine;
    engine = 0;
}

bool QDriveController::mount(const QString &device, const QString &path)
{
    bool result = true;
    FSVolumeRefNum refNum;
    CFURLRef mountUrl = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                                (const UInt8*)path.toUtf8().data(),
                                                                path.length(),
                                                                true);

    if (device.startsWith(QLatin1String("afp://")) ||
            device.startsWith(QLatin1String("smb://")) ||
            device.startsWith(QLatin1String("ftp://"))) {

        CFStringRef deviceString = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                                device.utf16(),
                                                                device.length());

        CFStringRef encodedString = CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,
                                                                            deviceString,
                                                                            0,
                                                                            0,
                                                                            0);

        CFURLRef url = CFURLCreateWithString(kCFAllocatorDefault,
                                             encodedString,
                                             0);

        OSStatus status = FSMountServerVolumeSync(url, mountUrl, 0, 0, &refNum, 0);
        if (status != noErr) {
            d->setError(status);
            result =  false;
        }

        if (url)
            CFRelease(url);
        CFRelease(encodedString);
        CFRelease(deviceString);

    } else {

        CFStringRef disk = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                        device.utf16(),
                                                        device.length());
        CFShow(disk);

        OSStatus status = FSMountLocalVolumeSync(disk, 0, &refNum, /*kFSIterateReserved*/0);
        if (status != noErr) {
            d->setError(status);
            result = false;
        }
        CFRelease(disk);
    }

    if (mountUrl)
        CFRelease(mountUrl);

    return result;
}

static FSVolumeRefNum getRefNumByPath(const QString &path)
{
    OSStatus result;

    FSRef fsref;
    result = FSPathMakeRef((const UInt8*)path.toUtf8().constData(), &fsref, 0);
    if (result != noErr) {
        return kFSInvalidVolumeRefNum;
    }

    FSCatalogInfo catalogInfo;
    result = FSGetCatalogInfo(&fsref, kFSCatInfoVolume, &catalogInfo, 0, 0, 0);
    if (result != noErr) {
        return kFSInvalidVolumeRefNum;
    }

    return catalogInfo.volume;
}

bool QDriveController::unmount(const QString &path)
{
    OSStatus result;

    FSVolumeRefNum refNum = getRefNumByPath(path);
    pid_t dissenter;

    // TODO: manually check refNum == kFSInvalidVolumeRefNum ??

    result = FSUnmountVolumeSync(refNum, 0, &dissenter);

    if (result != noErr) {
        d->setError(result);
        return false;
    }

    return true;
}

bool QDriveController::eject(const QString &path)
{
    OSStatus result;

    FSVolumeRefNum refNum = getRefNumByPath(path);
    pid_t dissenter;

    // TODO: manually check refNum == kFSInvalidVolumeRefNum ??

    result = FSEjectVolumeSync(refNum, 0, &dissenter);

    if (result != noErr) {
        d->setError(result);
        return false;
    }

    return true;
}

#include "qdrivecontroller_mac.moc"
