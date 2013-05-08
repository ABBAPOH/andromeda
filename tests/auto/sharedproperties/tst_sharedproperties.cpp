#include <QtCore/QString>
#include <QtTest/QtTest>

#include <GuiSystem/SharedProperties>

using namespace GuiSystem;

class Object : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
public:
    explicit Object(QObject *parent = 0) : QObject(parent) {}

    QString property() const { return m_property; }
    void setProperty(const QString &property)
    {
        if (m_property != property) {
            m_property = property;
            emit propertyChanged(property);
        }
    }

signals:
    void propertyChanged(const QString &);

private:
    QString m_property;
};

class SharedPropertiesTest : public QObject
{
    Q_OBJECT

public:
    SharedPropertiesTest();

private Q_SLOTS:
    void testGroup();
    void testSetValue();
    void testNotification();
};

SharedPropertiesTest::SharedPropertiesTest()
{
}

void SharedPropertiesTest::testSetValue()
{
    QString key = "property";
    QString value = "value";
    SharedProperties props;
    Object o1, o2;

    QCOMPARE(o1.property(), QString());
    QCOMPARE(o2.property(), QString());

    props.addObject(key, &o1);
    QCOMPARE(o1.property(), QString());
    props.setValue(key, value);
    QCOMPARE(o1.property(), value);

    props.addObject(key, &o2);
    QCOMPARE(o2.property(), value);
}

void SharedPropertiesTest::testNotification()
{
    QString key = "property";
    QString value = "value";
    SharedProperties props;
    Object o1, o2;

    QCOMPARE(o1.property(), QString());
    QCOMPARE(o2.property(), QString());

    props.addObject(key, &o1);
    QCOMPARE(o1.property(), QString());

    o1.setProperty(value);
    QCOMPARE(props.value(key).toString(), value);
    QCOMPARE(o1.property(), value);
    QCOMPARE(o2.property(), QString());

    props.addObject(key, &o2);
    QCOMPARE(o2.property(), value);
}

void SharedPropertiesTest::testGroup()
{
    QString key = "key";
    QString longKey = "group/key";
    QString group = "group";
    QVariant value = "value";

    SharedProperties props;

    QCOMPARE(props.value(key), QVariant());
    props.setValue(key, value);
    QCOMPARE(props.value(key), value);
    props.setValue(key, QVariant());

    QCOMPARE(props.value(longKey), QVariant());
    props.setValue(longKey, value);
    QCOMPARE(props.value(longKey), value);
    props.setValue(longKey, QVariant());

    props.beginGroup(group);
    QCOMPARE(props.group(), group);

    QCOMPARE(props.value(key), QVariant());
    props.setValue(key, value);
    QCOMPARE(props.value(key), value);
    props.endGroup();
    QCOMPARE(props.value(longKey), value);
}

QTEST_APPLESS_MAIN(SharedPropertiesTest)

#include "tst_sharedproperties.moc"
