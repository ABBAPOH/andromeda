#ifndef QTRASHFILEINFO_P_H
#define QTRASHFILEINFO_P_H

#include "qtrashfileinfo.h"

#include <QtCore/QSharedData>

class QTrashFileInfoData : public QSharedData
{
public:
    QString path;
    QString originalPath;

    QDateTime deletionDateTime;
    qint64 size;
};

#endif // QTRASHFILEINFO_P_H
