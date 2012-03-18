#include "qdrivecontroller.h"
#include "qdrivecontroller_p.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QDriveInfo>
#include <QDebug>

#define _WIN32_WINNT 0x0500
#include <qt_windows.h>
#include <dbt.h>

#ifndef DBT_CUSTOMEVENT
#  define DBT_CUSTOMEVENT 0x8006
#endif

#define QDRIVECONTROLLER_DEBUG 1

Q_CORE_EXPORT HINSTANCE qWinAppInst();

class QDriveWatcherEngine
{
public:
    QDriveWatcher *watcher;
    QSet<QString> drives;
    HWND hwnd;
};

static inline QStringList drivesFromMask(quint32 driveBits)
{
        QStringList ret;

        char driveName[] = "A:/";
        driveBits = (driveBits & 0x3ffffff);
        while (driveBits) {
            if (driveBits & 0x1)
                ret.append(QString::fromLatin1(driveName));
            ++driveName[0];
            driveBits = driveBits >> 1;
        }

        return ret;
}

LRESULT CALLBACK dw_internal_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DEVICECHANGE) {
        DEV_BROADCAST_HDR *lpdb = (DEV_BROADCAST_HDR *)lParam;
        switch (wParam) {
        case DBT_DEVNODES_CHANGED:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("DBT_DEVNODES_CHANGED message received, no extended info.");
#endif
            break;

        case DBT_QUERYCHANGECONFIG:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("DBT_QUERYCHANGECONFIG message received, no extended info.");
#endif
            break;
        case DBT_CONFIGCHANGED:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("DBT_CONFIGCHANGED message received, no extended info.");
#endif
            break;
        case DBT_CONFIGCHANGECANCELED:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("DBT_CONFIGCHANGECANCELED message received, no extended info.");
#endif
            break;

        case DBT_DEVICEARRIVAL:
        case DBT_DEVICEQUERYREMOVE:
        case DBT_DEVICEQUERYREMOVEFAILED:
        case DBT_DEVICEREMOVEPENDING:
        case DBT_DEVICEREMOVECOMPLETE:
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                DEV_BROADCAST_VOLUME *db_volume = (DEV_BROADCAST_VOLUME *)lpdb;
                QStringList drives = drivesFromMask(db_volume->dbcv_unitmask);
#ifdef GWLP_USERDATA
                QDriveWatcherEngine *engine = (QDriveWatcherEngine *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
//                QDriveWatcher *watcher = (QDriveWatcher *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
                QDriveWatcherEngine *engine = (QDriveWatcherEngine *)GetWindowLong(hwnd, GWL_USERDATA);
#endif
                QDriveWatcher *watcher = engine->watcher;

                if (wParam == DBT_DEVICEARRIVAL) {
                    foreach (const QString &drive, drives) {
#ifdef QDRIVECONTROLLER_DEBUG
                        if (db_volume->dbcv_flags & DBTF_MEDIA)
                            qWarning("Drive %c: Media has been arrived.", drive.at(0).toAscii());
                        else if (db_volume->dbcv_flags & DBTF_NET)
                            qWarning("Drive %c: Network share has been mounted.", drive.at(0).toAscii());
                        else
                            qWarning("Drive %c: Device has been added.", drive.at(0).toAscii());
#endif
                        if (engine->drives.contains(drive)) {
                            watcher->emitDriveRemoved(drive);
                        }
                        engine->drives.insert(drive);
                        watcher->emitDriveAdded(drive);
                    }
                } else if (wParam == DBT_DEVICEQUERYREMOVE) {
                } else if (wParam == DBT_DEVICEQUERYREMOVEFAILED) {
                } else if (wParam == DBT_DEVICEREMOVEPENDING) {
                } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                    foreach (const QString &drive, drives) {
#ifdef QDRIVECONTROLLER_DEBUG
                        if (db_volume->dbcv_flags & DBTF_MEDIA)
                            qWarning("Drive %c: Media has been removed.", drive.at(0).toAscii());
                        else if (db_volume->dbcv_flags & DBTF_NET)
                            qWarning("Drive %c: Network share has been unmounted.", drive.at(0).toAscii());
                        else
                            qWarning("Drive %c: Device has been removed.", drive.at(0).toAscii());
#endif
                        watcher->emitDriveRemoved(drive);
                        // fix for unmounting virtual drive using Daemon tools
                        if (QDriveInfo(drive).type() == QDriveInfo::CdromDrive) {
                            watcher->emitDriveAdded(drive);
                        }
                    }
                }
            }
            break;
        case DBT_DEVICETYPESPECIFIC:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("DBT_DEVICETYPESPECIFIC message received, may contain an extended info.");
#endif
            break;
        case DBT_CUSTOMEVENT:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("DBT_CUSTOMEVENT message received, contains an extended info.");
#endif
            break;
        case DBT_USERDEFINED:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("WM_DEVICECHANGE user defined message received, can not handle.");
#endif
            break;

        default:
#ifdef QDRIVECONTROLLER_DEBUG
            qWarning("WM_DEVICECHANGE message received, unhandled value %d.", wParam);
#endif
            break;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

static inline QString className()
{
    return QLatin1String("QDriveWatcherWin32_Internal_Widget") + QString::number(quintptr(dw_internal_proc));
}

static inline HWND dw_create_internal_window(const void* userData)
{
    QString className = ::className();
    HINSTANCE hi = qWinAppInst();

    WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = dw_internal_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hi;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = reinterpret_cast<const wchar_t *>(className.utf16());
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName,       // classname
                             wc.lpszClassName,       // window name
                             0,                      // style
                             0, 0, 0, 0,             // geometry
                             0,                      // parent
                             0,                      // menu handle
                             hi,                     // application
                             0);                     // windows creation data.
    if (!hwnd) {
        qWarning("QDriveWatcherEngine: Failed to create internal window: %d", (int)GetLastError());
    } else if (userData) {
#ifdef GWLP_USERDATA
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)userData);
#else
        SetWindowLong(hwnd, GWL_USERDATA, (LONG)userData);
#endif
    }

    return hwnd;
}

static inline void dw_destroy_internal_window(HWND hwnd)
{
    if (hwnd)
        DestroyWindow(hwnd);

    UnregisterClass(reinterpret_cast<const wchar_t *>(className().utf16()), qWinAppInst());
}

bool QDriveWatcher::start_sys()
{
    engine = new QDriveWatcherEngine;
    engine->watcher = this;
    foreach (const QDriveInfo &info, QDriveInfo::drives()) {
        engine->drives.insert(info.rootPath());
    }
    engine->hwnd = dw_create_internal_window(engine);
    return engine->hwnd;
}

void QDriveWatcher::stop_sys()
{
    if (engine) {
        dw_destroy_internal_window(engine->hwnd);
        delete engine;
        engine = 0;
    }
}

QString getEmptyLetter()
{
    char driveName[] = "Z:";
    quint32 driveBits = quint32(::GetLogicalDrives()) & 0x3ffffff;

    for (int i = 25; i >= 0; i--) {
        if (driveBits & (1 << i)) {
            driveName[0] -= 25 - i;
            break;
        }
    }
    return QLatin1String(driveName);
}

bool QDriveController::mount(const QString &device, const QString &path)
{
    QString targetPath = QDir::toNativeSeparators(path.isEmpty() ? getEmptyLetter() : path);
    if (targetPath.endsWith(QLatin1Char('\\')))
        targetPath = targetPath.left(targetPath.length()-1);

    if (device.startsWith(QLatin1String("\\\\?\\"))) { // GUID

        bool result = SetVolumeMountPoint((wchar_t*)targetPath.utf16(), (wchar_t*)device.utf16());
        if (!result) {
            d->setError(GetLastError());
            return false;
        }

    } else if (device.startsWith(QLatin1String("\\\\"))) { // network share

        NETRESOURCE resource;
        resource.dwType = RESOURCETYPE_ANY;
        resource.lpRemoteName = (wchar_t*)device.utf16();
        resource.lpLocalName = (wchar_t*)targetPath.utf16();
        resource.lpProvider = NULL;

        DWORD result = WNetAddConnection2(&resource, 0, 0, CONNECT_UPDATE_PROFILE);
        if (result != NO_ERROR) {
            d->setError(result);
            return false;
        }

    } else {

        QDriveInfo driveInfo(device);
        if (driveInfo.isValid()) {
            QString guid = driveInfo.device();
            return mount(guid, targetPath);
        } else {
            d->setError(ERROR_BAD_PATHNAME);
            return false;
        }

    }

    return true;
}

bool QDriveController::unmount(const QString &path)
{
    QString targetPath = QDir::toNativeSeparators(path);
    if (targetPath.startsWith(QLatin1String("\\\\")) ||
            QDriveInfo(path).type() == QDriveInfo::RemoteDrive) { // share

        if (targetPath.endsWith(QLatin1Char('\\')))
            targetPath.chop(1);

        DWORD result = WNetCancelConnection2((wchar_t*)targetPath.utf16(), CONNECT_UPDATE_PROFILE, true);
        if (result != NO_ERROR) {
            d->setError(result);
            return false;
        }

    } else {

        if (!targetPath.endsWith(QLatin1Char('\\')))
            targetPath.append(QLatin1Char('\\'));

        bool result = DeleteVolumeMountPoint((wchar_t*)targetPath.utf16());
        if (!result) {
            d->setError(GetLastError());
            return false;
        }

    }

    return true;
}

static bool ejectCDRom(bool eject, const QString &drive, QDriveControllerPrivate::Error &error)
{
    wchar_t buffer[255] = L"";
    MCI_OPEN_PARMS open;
    DWORD flags;

    ZeroMemory(&open, sizeof(MCI_OPEN_PARMS));

    open.lpstrDeviceType = (LPWSTR) MCI_DEVTYPE_CD_AUDIO;
    open.lpstrElementName = (wchar_t*)drive.utf16();

    flags = MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID;

    MCIERROR result = mciSendCommand(0, MCI_OPEN, flags, (DWORD) &open);
    if (result == 0) {
        result = mciSendCommand(open.wDeviceID, MCI_SET, (eject) ? MCI_SET_DOOR_OPEN : MCI_SET_DOOR_CLOSED, 0);
        mciSendCommand(open.wDeviceID, MCI_CLOSE, MCI_WAIT, 0);
    }

    if (result) {
        mciGetErrorString(result, buffer, 255);
        error.code = result;
        error.string = QString::fromWCharArray(buffer, 255);
        return false;
    }

    return true;
}

bool QDriveController::eject(const QString &path)
{
    QDriveInfo info(path);
    if (info.type() == QDriveInfo::CdromDrive)
        return ejectCDRom(true, info.rootPath(), d->error);

    return unmount(path);
}
