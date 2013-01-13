#ifndef PLUGINSPECFORMATHANDLER_P_H
#define PLUGINSPECFORMATHANDLER_P_H

#include <QtCore/QString>

#include "pluginspec_p.h"

namespace ExtensionSystem {

class PluginSpecFormatHandler
{
public:
    PluginSpecFormatHandler() { clearError(); }
    virtual ~PluginSpecFormatHandler() {}

    virtual bool canRead(QIODevice *device) = 0;

    virtual bool read(QIODevice *device, PluginSpecPrivate *d) = 0;
    virtual bool write(QIODevice *device, PluginSpecPrivate *d) = 0;

    QString errorString() const { return m_errorString; }

protected:
    inline void setErrorString(const QString &errorString) { m_errorString = errorString; }
    inline void clearError() { m_errorString = QObject::tr("No error"); }

private:
    QString m_errorString;
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
