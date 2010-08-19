#ifndef IPLUGIN_P_H
#define IPLUGIN_P_H

#include "iplugin.h"

namespace ExtensionSystem {
class IPluginPrivate
{
public:
    IPluginPrivate();
    virtual ~IPluginPrivate();

    QList<QObject *> addedObjects;
};
} // namespace ExtensionSystem

#endif // IPLUGIN_P_H
