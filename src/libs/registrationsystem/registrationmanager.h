#ifndef REGISTRATIONMANAGER_H
#define REGISTRATIONMANAGER_H

#include <QObject>

namespace RegistrationSystem
{

class IRegistrator
{
public:
    virtual bool canRegister(QObject *object) { return true; }
    virtual bool registerObject(QObject *object) = 0;
};

class RegistrationManagerPrivate;
class RegistrationManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RegistrationManager)
    Q_DISABLE_COPY(RegistrationManager)
public:
    explicit RegistrationManager(QObject *parent = 0);
    ~RegistrationManager();

    void addRegistrator(IRegistrator *registrator, const QString &type = "");
    void removeRegistrator(IRegistrator *registrator);

signals:
    void objectRegistered(QObject *object);

public slots:
    void registerObject(QObject *object, const QString &type = "");

protected:
    RegistrationManagerPrivate *d_ptr;
};

} // namespace RegistrationSystem

#endif // REGISTRATIONMANAGER_H
