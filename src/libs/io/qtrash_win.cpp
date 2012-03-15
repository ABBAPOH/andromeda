#include "qtrash.h"
#include "qtrash_p.h"

#include <Windows.h>
#include <Shellapi.h>
#include <Winerror.h>

// Hack to make functins available on win xp
#undef _WIN32_IE
#define _WIN32_IE 0x0500
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#include <Shlobj.h>
#undef WINVER
#define WINVER 0x500
#include <Sddl.h>

#include <QTime>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QDebug>

#include "info2_p.h"
#include "qdriveinfo.h"
#include "qtrashfileinfo_p.h"

static const qint64 SECONDS_SINCE_1601 =  11644473600LL;
static const qint64 USEC_IN_SEC        =      1000000LL;

static inline time_t filetimeToTime_t(const FILETIME *time)
{
    ULARGE_INTEGER i64;
    i64.LowPart   = time->dwLowDateTime;
    i64.HighPart  = time->dwHighDateTime;
    i64.QuadPart /= USEC_IN_SEC * 10;
    i64.QuadPart -= SECONDS_SINCE_1601;
    return i64.QuadPart;
}

static void updateVistaInfo(const QString &trash, const QString &baseName)
{
    QString infoFile = trash + QLatin1Char('/') + QLatin1String("$I") + baseName.mid(2);
    QFile::remove(infoFile);
}

static void updateInfo(const QString &path)
{
    QFileInfo info(path);
    QString trash = info.path();
    QString fileName = info.fileName();
    if (fileName.startsWith(QLatin1String("$R")))
        updateVistaInfo(trash, fileName);
    else
        updateInfo2(trash, path);
}

static bool doFileOp(const QString &path, UINT wFunc, FILEOP_FLAGS fFlags)
{
    // remove first '/' - can't use toLocalFile() because scheme is not file://
    const QString nativePath = QDir::toNativeSeparators(path);
    // must be double-null terminated.
    QByteArray delBuf( ( nativePath.length() + 2 ) * 2, 0 );
    memcpy( delBuf.data(), nativePath.utf16(), nativePath.length() * 2 );

    SHFILEOPSTRUCTW op;
    memset(&op, 0, sizeof(SHFILEOPSTRUCTW));
    op.wFunc = wFunc;
    op.pFrom = (LPCWSTR)delBuf.constData();
    op.fFlags = fFlags|FOF_NOCONFIRMATION|FOF_NOERRORUI;
    return SHFileOperationW(&op) == S_OK;
}

static int indexOfOriginalPath(const QTrashFileInfoList &files, const QString &path)
{
    for (int i = 0; i < files.size(); i++) {
        if (files.at(i).originalPath() == path)
            return i;
    }

    return -1;
}

static int indexOfTrashPath(const QTrashFileInfoList &files, const QString &path)
{
    for (int i = 0; i < files.size(); i++) {
        if (files.at(i).path() == path)
            return i;
    }

    return -1;
}

static QString getUserSID()
{
    wchar_t *user_name = 0;
    wchar_t *dom = 0;
    wchar_t *sid_str = 0;
    SID_NAME_USE use;
    PSID psid = 0;
    DWORD size, dom_size;

    size = 0;
    GetUserName(0, &size);

    user_name = new wchar_t[size + 1];
    if (!user_name)
        return QString();

    if (!GetUserNameW(user_name, &size))
       goto done;

    size = 0;
    dom_size = 0;
    LookupAccountName(0, user_name, 0, &size, 0, &dom_size, &use);

    psid = (PSID) malloc(size);

    dom = new wchar_t[dom_size];

    if (!psid || !dom)
        goto done;

    if (!LookupAccountName(0, user_name, psid, &size, dom, &dom_size, &use))
       goto done;

    if (!ConvertSidToStringSid(psid, &sid_str))
       goto done;

done:
    delete user_name;
    delete(dom);
    free(psid);

    QString result = QString::fromWCharArray(sid_str);
    free(sid_str);
    return result;
}

static inline QString getTrash(const QString &rootPath)
{
    return rootPath + QLatin1String("RECYCLER") + QLatin1Char('/') + getUserSID();
}

QTrashPrivate::QTrashPrivate()
{
    // get trash IShellFolder object
    LPITEMIDLIST  iilTrash;
    IShellFolder *isfDesktop;

    // we assume that this will always work - if not we've a bigger problem than a kio_trash crash...
    SHGetFolderLocation(0, CSIDL_BITBUCKET, 0, 0, &iilTrash);
    SHGetDesktopFolder( &isfDesktop );
    isfDesktop->BindToObject( iilTrash, 0, IID_IShellFolder2, (void**)&m_isfTrashFolder );
    isfDesktop->Release();
    SHGetMalloc(&m_pMalloc);
}

bool QTrash::moveToTrash(const QString &path, QString *trashPath)
{
    QTime t;
    t.start();

    bool result = false;
    // if SHFileOperation will be slow on large folders, we can do the trick:
    // rename original folder, create temp file with the name as folder,
    // delete it using SHFileOperation, then remove it from trash foder
    // and move original folder to trah using new name
    result = doFileOp(path, FO_DELETE, FOF_ALLOWUNDO);

    qDebug() << "moveToTrash" << "elapsed" << t.elapsed();

    if (trashPath) {
        QTrashFileInfoList files = this->files();
        int index = indexOfOriginalPath(files, path);
        if (index != -1)
            *trashPath = files.at(index).path();
    }

    return result;
}

bool QTrash::restore(const QString &trashPath)
{
    QTrashFileInfoList files = this->files();
    int index = indexOfTrashPath(files, trashPath);
    if (index == -1)
        return false;

    // We can use COM objets to receive Trash context menu and ask Recycle Bin to
    // restore file. However, it is much easier way to restore file.
    QString originalPath = files.at(index).originalPath();
    bool ok = QFile::rename(trashPath, originalPath);
    qDebug() << "restore" << trashPath << originalPath << ok;
    if (ok)
        updateInfo(trashPath);
    return ok;
}

bool QTrash::remove(const QString &trashPath)
{
    // we can't use SHFileOperation because it doesn't updates INFO2 file
    bool ok = QFile::remove(trashPath);
    if (!ok)
        return false;

    updateInfo(trashPath);

    return ok;
}

static inline QString fromWinPath(wchar_t *path)
{
    return QDir::fromNativeSeparators(QString::fromUtf16((const unsigned short*)path));
}

QTrashFileInfoList QTrashPrivate::getFiles(const QString &trash) const
{
    QTrashFileInfoList result;

    IEnumIDList *l;
    HRESULT res = m_isfTrashFolder->EnumObjects(0, SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN, &l);
    if (res != S_OK)
        return QTrashFileInfoList();

    STRRET strret;
    SFGAOF attribs;
    QTrashFileInfoData data;

    LPITEMIDLIST i;
    WIN32_FIND_DATAW findData;
    while (l->Next( 1, &i, 0) == S_OK) {
        m_isfTrashFolder->GetDisplayNameOf(i, SHGDN_FORPARSING, &strret);
        data.path = fromWinPath(strret.pOleStr);
        m_pMalloc->Free(strret.pOleStr);

        // add only files that satisfies specifed trash
        if (!data.path.startsWith(trash)) {
            ILFree(i);
            continue;
        }

        m_isfTrashFolder->GetDisplayNameOf(i, SHGDN_NORMAL, &strret);
        data.originalPath = fromWinPath(strret.pOleStr);
        m_pMalloc->Free(strret.pOleStr);

        m_isfTrashFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&i, &attribs);
        SHGetDataFromIDList(m_isfTrashFolder, i, SHGDFIL_FINDDATA, &findData, sizeof(findData));

        data.size = ((quint64)findData.nFileSizeLow) + (((quint64)findData.nFileSizeHigh) << 32);
        // i hope this is correct:
        data.deletionDateTime.setTime_t(filetimeToTime_t(&findData.ftLastWriteTime));

        ILFree(i);

        result.append(QTrashFileInfo(data));
    }

    l->Release();

    return result;
}

// TODO: maybe we need to cache files
QTrashFileInfoList QTrash::files() const
{
    Q_D(const QTrash);

    return d->getFiles();
}

QTrashFileInfoList QTrash::files(const QString &trash) const
{
    Q_D(const QTrash);

    return d->getFiles(trash);
}

QStringList QTrash::trashes() const
{
    QStringList result;

    foreach (const QDriveInfo &drive, QDriveInfo::drives()) {
        QString rootPath = drive.rootPath();
        if (rootPath == QLatin1String("A:/") || rootPath == QLatin1String("B:/"))
            continue;

        QString trash = getTrash(rootPath);
        if (QFileInfo(trash).exists())
            result.append(trash);
    }

    return result;
}

void QTrash::clearTrash(const QString &trash)
{
    SHEmptyRecycleBin(0, (wchar_t*)trash.unicode(), SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
}

void QTrash::clearTrash()
{
    SHEmptyRecycleBin(0, 0, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
}
