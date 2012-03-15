#ifndef INFO2_H
#define INFO2_H

#include <QtCore/QDataStream>

struct INFO2Header
{
    quint32 reserved0; // dunno for what this
    quint32 reserved1;
    quint32 reserved2;
    quint32 recordSize; // always 0x0320
    quint32 totalLogicalSize;
};

struct INFO2Record
{
//    quint32 reserved1;
    QByteArray localName;
    quint32 number;
    quint32 drive;
    quint64 deletionTime;
    quint32 fileSize;
    QString unicodeName;
};

void updateInfo2(const QString &trash, const QString &exclude);

#endif // INFO2_H
