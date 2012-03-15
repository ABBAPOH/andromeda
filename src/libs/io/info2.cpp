#include "info2_p.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtEndian>

enum {
  LEGACY_FILENAME_OFFSET  = 0x4 - 4,
  RECORD_INDEX_OFFSET     = 0x108 - 4,
  DRIVE_LETTER_OFFSET     = 0x10C - 4,
  FILETIME_OFFSET         = 0x110 -4,
  FILESIZE_OFFSET         = 0x118 - 4,
  UNICODE_FILENAME_OFFSET = 0x11C - 4,
  RECORD_LENGTH           = 0x320
};

static QDataStream & operator << (QDataStream &s, const INFO2Header &h)
{
    s << h.reserved0;
    s << h.reserved1;
    s << h.reserved2;
    s << h.recordSize;
    s << h.totalLogicalSize;
    return s;
}

QDataStream & operator >> (QDataStream &s, INFO2Header &h)
{
    s >> h.reserved0;
    s >> h.reserved1;
    s >> h.reserved2;
    s >> h.recordSize;
    s >> h.totalLogicalSize;
    return s;
}

QDataStream & operator >> (QDataStream &s, INFO2Record &r)
{
    r.localName = s.device()->read(RECORD_INDEX_OFFSET - LEGACY_FILENAME_OFFSET);
    s >> r.number;
    s >> r.drive;
    s >> r.deletionTime;
    s >> r.fileSize;
    r.unicodeName = QString::fromUtf16((ushort*)s.device()->read(RECORD_LENGTH - UNICODE_FILENAME_OFFSET).data());
    return s;
}

QDataStream & operator << (QDataStream &s, const INFO2Record &r)
{
    QByteArray name = r.localName;
    name += QByteArray(RECORD_INDEX_OFFSET - LEGACY_FILENAME_OFFSET - r.localName.length(), 0);
    Q_ASSERT(name.length() == RECORD_INDEX_OFFSET - LEGACY_FILENAME_OFFSET);
    s.device()->write(name.data(), name.length());

    s << r.number;
    s << r.drive;
    s << r.deletionTime;
    s << r.fileSize;

    QByteArray unicodeName = QByteArray((char*)r.unicodeName.data(), r.unicodeName.length()*2);
    unicodeName += QByteArray(RECORD_LENGTH - UNICODE_FILENAME_OFFSET - r.unicodeName.length()*2, 0);
    s.device()->write(unicodeName.data(), unicodeName.length());

    return s;
}

void updateInfo2(const QString &trash, const QString &exclude)
{
    // exclude path is in format D<drive><number>
    QString fileName = QFileInfo(exclude).fileName();
    int dotIndex = fileName.indexOf(QLatin1Char('.'));
    uint drive = fileName.at(1).unicode() - QChar(QLatin1Char('A')).unicode();
    uint number = fileName.mid(2, dotIndex - 2).toInt();

    QFile f(trash + "/INFO2");
    if (!f.open(QFile::ReadOnly))
        return;

    QBuffer buf;
    buf.open(QBuffer::WriteOnly);

    QDataStream s(&f);
    s.setByteOrder(QDataStream::LittleEndian);

    QDataStream s2(&buf);
    s2.setByteOrder(QDataStream::LittleEndian);

    INFO2Header h;
    s >> h;
    s2 << h;

    while (!f.atEnd()) {
        INFO2Record r;
        s >> r;
        if (r.number != number && r.drive != drive)
            s2 << r;
    }
    buf.close();
    buf.open(QBuffer::ReadOnly);
    f.close();

    f.open(QFile::WriteOnly);
    f.write(buf.data());
}
