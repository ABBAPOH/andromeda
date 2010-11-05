#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "extensionsystem_global.h"
#include "pluginspec.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ExtensionSystem {
class IPluginPrivate;
class PluginSpec;

class EXTENSIONSYSTEM_EXPORT IPlugin : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(IPlugin)

public:
    IPlugin();
    virtual ~IPlugin();

    virtual bool initialize() = 0;
    virtual QList<PluginDependency> dependencies() { return QList<PluginDependency>(); }
    virtual void shutdown() {}

    void addObject(QObject * object);
    void addObjectByName(QObject * object, const QString name);
    void addObjectByType(QObject * object, const QString type);

    void removeObject(QObject * object);

protected:
    IPluginPrivate *d_ptr;
};

} // namespace ExtensionSystem

#endif // IPLUGIN_H
