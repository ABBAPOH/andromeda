#ifndef QDRIVEINFO_H
#define QDRIVEINFO_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QString>

#ifdef QDRIVEINFO_DLL
#  include <QtCore/QtGlobal>
#  ifdef QDRIVEINFO_MAKEDLL
#    define QDRIVEINFO_EXPORT Q_DECL_EXPORT
#  else
#    define QDRIVEINFO_EXPORT Q_DECL_IMPORT
#  endif
#endif
#ifndef QDRIVEINFO_EXPORT
#  define QDRIVEINFO_EXPORT
#endif

class QDriveInfoPrivate;
class QDRIVEINFO_EXPORT QDriveInfo
{
public:
    enum DriveType {
        InvalidDrive = 0,
        InternalDrive,
        RemovableDrive,
        RemoteDrive,
        CdromDrive,
        InternalFlashDrive,
        RamDrive
    };

    QDriveInfo();
    explicit QDriveInfo(const QString &rootPath);
    QDriveInfo(const QDriveInfo &other);
    ~QDriveInfo();

    QDriveInfo &operator=(const QDriveInfo &other);

    bool operator==(const QDriveInfo &other) const;
    inline bool operator!=(const QDriveInfo &other) const;

    QString rootPath() const;
    void setRootPath(const QString &);

    QByteArray device() const;
    QByteArray fileSystemName() const;
    QString name() const;

    quint64 bytesTotal() const;
    quint64 bytesFree() const;
    quint64 bytesAvailable() const;

    inline bool isRoot() const;
    bool isReadOnly() const;
    bool isReady() const;
    bool isValid() const;

    DriveType type() const;

    void refresh();

    static QList<QDriveInfo> drives();
    static QDriveInfo rootDrive();

protected:
    QDriveInfoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QDriveInfo);
};

inline bool QDriveInfo::operator!=(const QDriveInfo &other) const
{ return !(operator==(other)); }

inline bool QDriveInfo::isRoot() const
{ return *this == QDriveInfo::rootDrive(); }

#endif // QDRIVEINFO_H
