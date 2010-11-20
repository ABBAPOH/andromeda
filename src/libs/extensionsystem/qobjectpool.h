#ifndef QOBJECTPOOL_H
#define QOBJECTPOOL_H

#include "extensionsystem_global.h"

#include <QtCore/QObject>

namespace ExtensionSystem {

class QObjectPoolPrivate;
class EXTENSIONSYSTEM_EXPORT QObjectPool : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QObjectPool)
    Q_DISABLE_COPY(QObjectPool)

public:
    explicit QObjectPool(QObject *parent = 0);
    virtual ~QObjectPool();

    Q_INVOKABLE void addObject(QObject * object, const QString &type = "");
    Q_INVOKABLE void removeObject(QObject * object);

    QObject * object(const QString &name) const;
    QObjectList objects(const QString &name) const;

    template <class T> T * object(const QString &name) const
    {
        return qobject_cast<T *>(object(name));
    }

    QObjectList objects() const;

    template <class T> T* object() const
    {
        foreach (QObject * object, objects()) {
            T * t = qobject_cast<T*>(object);
            if (t)
                return t;
            return 0;
        }
    }

    template <class T> QList<T*> objects() const
    {
        QList<T*> result;
        foreach (QObject * object, objects()) {
            T * t = qobject_cast<T*>(object);
            if (t)
                result.append(t);
        }
        return result;
    }

signals:
    void objectAdded(QObject *object, const QString &type);
    void objectRemoved(QObject *object);

public slots:

protected:
    QObjectPool(QObjectPoolPrivate &dd, QObject* parent = 0);

    QObjectPoolPrivate *d_ptr;
};

} // namespace ExtensionSystem

#endif // QOBJECTPOOL_H
