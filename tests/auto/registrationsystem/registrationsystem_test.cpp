
#include <QtTest/QtTest>
#include <QObject>
#include <registrationmanager.h>
#include <QString>

using namespace RegistrationSystem;

class Object: public QObject
{
    Q_OBJECT
public:
};

class Registrator: public IRegistrator
{
public:
    virtual bool canRegister(QObject *object)
    {
        return qobject_cast<Object*>(object);
    }
    virtual bool registerObject(QObject *object)
    {
        object->setObjectName("registered");
        return true;
    };
    virtual bool unregisterObject(QObject *object)
    {
        object->setObjectName("unregistered");
        return true;
    };
};

class TestRegistrationSystem: public QObject
{
    Q_OBJECT
    RegistrationManager *manager;
    Registrator *registrator;
private slots:
    void initTestCase();
    void test1();
};

void TestRegistrationSystem::initTestCase()
{
    manager = new RegistrationManager();
    registrator = new Registrator();
}

void TestRegistrationSystem::test1()
{
    manager->addRegistrator(registrator, "test");
    Object *object1 = new Object;
    manager->registerObject(object1, "test");
    QCOMPARE(object1->objectName(), QString("registered"));
    manager->unregisterObject(object1);
    QCOMPARE(object1->objectName(), QString("unregistered"));
}

QTEST_MAIN(TestRegistrationSystem)
#include "registrationsystem_test.moc"
