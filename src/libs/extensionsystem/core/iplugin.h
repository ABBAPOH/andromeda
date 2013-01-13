#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "../extensionsystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QMap>

class QVariant;
typedef QMap<QString, QVariant> QVariantMap;

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
    virtual void postInitialize(const QVariantMap &options) { Q_UNUSED(options); }
    virtual void shutdown() {}

protected:
    QObject *object(const QString &name);
    template <class T> T* object(const QString &name) { return qobject_cast<T*>(object(name)); }

    void addObject(QObject *object, const QString &name = "");
    void removeObject(QObject *object);

protected:
    IPluginPrivate *d_ptr;
};

} // namespace ExtensionSystem

#endif // IPLUGIN_H
