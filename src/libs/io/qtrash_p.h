#ifndef QTRASH_P_H
#define QTRASH_P_H

#include "qtrash.h"

class IShellFolder2;
class IMalloc;

class QTrashPrivate
{
public:
    QTrashPrivate();

    bool removePath(const QString &path);

public:
//    bool isDirty;
//    QTrashFileInfoList files;
#ifdef Q_OS_MAC
    // TODO: implement thread-safety
    static QMap<QString, QTrashFileInfo> deletedFiles;
#endif

#ifdef Q_OS_WIN
    QTrashFileInfoList getFiles(const QString &trash = QString()) const;

    IShellFolder2 *m_isfTrashFolder;
    IMalloc *m_pMalloc;
#endif

};

#endif // QTRASH_P_H
