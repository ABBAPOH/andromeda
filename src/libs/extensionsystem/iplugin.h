#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "extensionsystem_global.h"

#include <QtCore/QObject>

namespace ExtensionSystem {

class IPluginPrivate;
class EXTENSIONSYSTEM_EXPORT IPlugin : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(IPlugin)
    Q_DISABLE_COPY(IPlugin)

public:
    IPlugin();
    virtual ~IPlugin();

    virtual bool initialize() = 0;
    virtual void shutdown() {}

protected:
    void addObject(QObject *object, const QString &name = "");
    void removeObject(QObject *object);

protected:
    IPluginPrivate *d_ptr;
};

} // namespace ExtensionSystem

#endif // IPLUGIN_H
