#ifndef PLUGINSPECFORMATHANDLER_P_H
#define PLUGINSPECFORMATHANDLER_P_H

#include <QtCore/QString>

#include "pluginspec_p.h"

namespace ExtensionSystem {

class PluginSpecFormatHandler
{
public:
    PluginSpecFormatHandler() {}
    virtual ~PluginSpecFormatHandler() {}

    virtual bool canRead(QIODevice *device) = 0;

    virtual bool read(QIODevice *device, PluginSpecPrivate *d) = 0;
    virtual bool write(QIODevice *device, PluginSpecPrivate *d) = 0;
};

class PluginSpecBinaryHandler : public PluginSpecFormatHandler
{
    bool canRead(QIODevice *device);

    bool read(QIODevice *device, PluginSpecPrivate *d);
    bool write(QIODevice *device, PluginSpecPrivate *d);
};

class PluginSpecXmlHandler : public PluginSpecFormatHandler
{
    bool canRead(QIODevice *device);

    bool read(QIODevice *device, PluginSpecPrivate *d);
    bool write(QIODevice *device, PluginSpecPrivate *d);
};

} // namespace ExtensionSystem

#endif // PLUGINSPECFORMATHANDLER_P_H
