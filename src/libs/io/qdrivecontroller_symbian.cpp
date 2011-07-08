#include "qdrivecontroller.h"
#include "qdrivecontroller_p.h"

#ifndef SYMBIAN_3_1
#include <driveinfo.h>
#endif

QDriveWatcherEngine::QDriveWatcherEngine(QDriveWatcher *watcher)
    : CActive(EPriorityStandard),
      m_watcher(watcher)
{
    CActiveScheduler::Add(this);

    if (iFs.Connect() == KErrNone) {
#ifndef SYMBIAN_3_1
        m_previousDriveList.Copy(PopulateDriveList());
#endif
        startMonitoring();
    }
}

QDriveWatcherEngine::~QDriveWatcherEngine()
{
    Cancel();
    iFs.Close();
}

void QDriveWatcherEngine::DoCancel()
{
    iFs.NotifyChangeCancel();
}

void QDriveWatcherEngine::RunL()
{
#ifdef SYMBIAN_3_1
    TDriveInfo driveInfo;
    TDriveNumber driveLetter = EDriveE;  // Have to use hardcoded MMC drive letter for 3.1
    if (iFs.Drive(driveInfo, driveLetter) == KErrNone) {
        bool driveInserted = driveInfo.iType != EMediaNotPresent;

        TChar volumeChar;
        QString volume;
        if (RFs::DriveToChar(driveLetter, volumeChar) == KErrNone)
            volume = QChar(volumeChar).toAscii();

        foreach (MStorageStatusObserver *observer, m_observers)
            observer->storageStatusChanged(driveInserted, volume);
    }
#else
    CompareDriveLists(PopulateDriveList());
#endif
    startMonitoring();
}

#ifndef SYMBIAN_3_1
TDriveList QDriveWatcherEngine::PopulateDriveList()
{
    TDriveList driveList;
    TInt driveCount = 0;
    if (DriveInfo::GetUserVisibleDrives(iFs, driveList, driveCount,
            KDriveAttExclude | KDriveAttRemote | KDriveAttRom | KDriveAttInternal) != KErrNone) {
        return TDriveList();
    }

    for (int i = 0; i < KMaxDrives; ++i) {
        if (driveList[i] == KDriveAbsent)
            continue;

        TUint driveStatus;
        if (DriveInfo::GetDriveStatus(iFs, i, driveStatus) == KErrNone) {
            if (!(driveStatus & DriveInfo::EDriveRemovable)) {
                driveList[i] = KDriveAbsent;
                continue;
            }

            TDriveInfo driveInfo;
            if (iFs.Drive(driveInfo, i) == KErrNone) {
                if (driveInfo.iType == EMediaNotPresent)
                    driveList[i] = KDriveAbsent;
            }
        }
    }
    return driveList;
}

void QDriveWatcherEngine::CompareDriveLists(const TDriveList &aDriveList)
{
    if (!(aDriveList.Length() > KMaxDrives - 1) || !(m_previousDriveList.Length() > KMaxDrives - 1))
        return;

    for (int i = 0; i < KMaxDrives; i++) {
        if (aDriveList[i] == KDriveAbsent && m_previousDriveList[i] == KDriveAbsent)
            continue;

        if (aDriveList[i] > KDriveAbsent && m_previousDriveList[i] > KDriveAbsent)
            continue;

        if (aDriveList[i] == KDriveAbsent && m_previousDriveList[i] > KDriveAbsent) {
            TChar volumeChar;
            QString volume;
            if (iFs.DriveToChar(i, volumeChar) == KErrNone)
                volume = QChar(volumeChar).toAscii();

            m_watcher->emitDriveRemoved(volume);
        } else if (aDriveList[i] > KDriveAbsent && m_previousDriveList[i] == KDriveAbsent) {
            TChar volumeChar;
            QString volume;
            if (iFs.DriveToChar(i, volumeChar) == KErrNone)
                volume = QChar(volumeChar).toAscii();

            m_watcher->emitDriveAdded(volume);
        }
    }
    m_previousDriveList.Copy(aDriveList);
}
#endif //SYMBIAN_3_1

void QDriveWatcherEngine::startMonitoring()
{
    iFs.NotifyChange(ENotifyDisk, iStatus);
    SetActive();
}


bool QDriveWatcher::start_sys()
{
    engine = new QDriveWatcherEngine;
    return true;
}

void QDriveWatcher::stop_sys()
{
    delete engine;
    engine = 0;
}
