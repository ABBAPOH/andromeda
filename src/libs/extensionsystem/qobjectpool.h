#ifndef QOBJECTPOOL_H
#define QOBJECTPOOL_H

#include <QtCore/QObject>

class QObjectPoolPrivate;
class QObjectPool : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QObjectPool)
    Q_DISABLE_COPY(QObjectPool)
public:
    explicit QObjectPool(QObject *parent = 0);
    virtual ~QObjectPool();

    void addObject(QObject * object, const QString &type = "");
    void removeObject(QObject * object);

    QObject * object(const QString &name);
    QObjectList objects(const QString &name);

    QObjectList objects();

    template <class T> T* object()
    {
        foreach (QObject * object, objects()) {
            T * t = qobject_cast<T*>(object);
            if (t)
                return t;
            return 0;
        }
    }

    template <class T> QList<T*> objects()
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

#endif // QOBJECTPOOL_H
