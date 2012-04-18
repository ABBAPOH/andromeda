#ifndef QPROGRAMINFO_P_H
#define QPROGRAMINFO_P_H

#include "qprograminfo.h"

#include <QtCore/QSharedData>

class QProgramInfoData : public QSharedData
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

#endif // QPROGRAMINFO_P_H
