#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

#include "pluginspec.h"

namespace ExtensionSystem {

class PluginSpec;
class PluginManagerPrivate;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PluginManager)
    Q_DECLARE_PRIVATE(PluginManager)
    Q_PROPERTY(QString pluginsFolder READ pluginsFolder WRITE setPluginsFolder)

public:
    static PluginManager *instance();

    explicit PluginManager(QObject *parent = 0);
    virtual ~PluginManager();

    void addObject(QObject * object);
    void removeObject(QObject * object);
    QObjectList objects();
    QObject * object(const QString &name);

    template <class T> T* object()
    {
        QReadLocker l(&m_lock);
        foreach (QObject * object, objects()) {
            T * t = qobject_cast<T*>(object);
            if (t)
                return t;
            return 0;
        }
    }

    QObjectList objects(const QString &name);
    template <class T> QList<T*> objects()
    {
        QReadLocker l(&m_lock);
        QList<T*> result;
        foreach (QObject * object, objects()) {
            T * t = qobject_cast<T*>(object);
            if (t)
                result.append(t);
        }
        return result;
    }

    void loadPlugins();
    void shutdown();

    QString pluginsFolder();
    void setPluginsFolder(const QString &name);

    QList<PluginSpec *> plugins() const;

signals:
    void pluginsChanged();
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

public slots:
    void updateDirectory(const QString &);
    void updateLibrary(const QString &);

protected:
    void timerEvent(QTimerEvent *event);

private:
    PluginManagerPrivate *d_ptr;
    static PluginManager *m_instance;
    mutable QReadWriteLock m_lock;
};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
