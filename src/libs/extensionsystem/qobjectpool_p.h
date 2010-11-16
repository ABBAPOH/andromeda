#ifndef QOBJECTPOOL_P_H
#define QOBJECTPOOL_P_H

#include "qobjectpool.h"

#include <QtCore/QHash>

class QObjectPoolPrivate
{
public:
    QObjectPoolPrivate();
    virtual ~QObjectPoolPrivate();

    QObjectList objects;
    QHash<QString, QObject *> namedObjects;
};

#endif // QOBJECTPOOL_P_H
