#ifndef QDEFAULTPROGRAM_P_H
#define QDEFAULTPROGRAM_P_H

#include "qdefaultprogram.h"

#include <QtCore/QSharedData>

class QDefaultProgramData : public QSharedData
{
public:
    bool valid;
    QString comment;
    QString copyright;
    QString genericName;
    QIcon icon;
    QString identifier;
    QString name;
    QString path;
    QString version;
};

class QDefaultProgramPrivate
{
public:
    static void removeDuplicates(QDefaultProgramList &list);
};

#endif // QDEFAULTPROGRAM_P_H
