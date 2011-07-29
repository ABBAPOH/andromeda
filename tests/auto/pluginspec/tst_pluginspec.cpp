#include <QtCore/QString>
#include <QtTest/QtTest>

class PluginSpecTest : public QObject
{
    Q_OBJECT

public:
    PluginSpecTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSerialise();
    void testSerialise_data();
};

PluginSpecTest::PluginSpecTest()
{
}

void PluginSpecTest::initTestCase()
{
}

void PluginSpecTest::cleanupTestCase()
{
}

void PluginSpecTest::testSerialise()
{
    qDebug() << "PluginSpecTest::testSerialise";
    QFETCH(QString, data);
    QFETCH(QString, data2);
    qDebug() << data << data2;
//    QVERIFY2(true, "Failure");
}

void PluginSpecTest::testSerialise_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<QString>("data2");
    QTest::newRow("0") << QString("hello world") << "blas";
    QTest::newRow("1") << QString("hello world") << "lol";
}

QTEST_APPLESS_MAIN(PluginSpecTest)

#include "tst_pluginspec.moc"
