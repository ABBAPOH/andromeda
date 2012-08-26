#include <QtCore/QString>
#include <QtTest/QtTest>

#include <sharedproperties.h>

using namespace GuiSystem;

class SharedPropertiesTest : public QObject
{
    Q_OBJECT

public:
    SharedPropertiesTest();

private Q_SLOTS:
    void testGroup();
    void testSetValue();
};

SharedPropertiesTest::SharedPropertiesTest()
{
}

void SharedPropertiesTest::testSetValue()
{
    QString key = "objectName";
    QString name = "objectName";
    SharedProperties props;
    QObject o1, o2;

    QCOMPARE(o1.objectName(), QString());
    QCOMPARE(o2.objectName(), QString());

    props.addObject(key, &o1);
    QCOMPARE(o1.objectName(), QString());
    props.setValue(key, name);
    QCOMPARE(o1.objectName(), name);

    props.addObject(key, &o2);
    QCOMPARE(o2.objectName(), name);

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
    props.setValue(longKey, QVariant());
}

QTEST_APPLESS_MAIN(SharedPropertiesTest)

#include "tst_sharedproperties.moc"
