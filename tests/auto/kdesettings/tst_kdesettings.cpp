#include <QtCore/QString>
#include <QtTest/QtTest>

#include <kdesettings.h>

class TestSettings: public QObject
{
    Q_OBJECT
public:
    TestSettings(QObject *parent=0);

private slots:
    void testIni();
    void testArray();
    void testBug158();
};

TestSettings::TestSettings(QObject *parent) :
    QObject(parent)
{
}

void TestSettings::testIni()
{
    KDESettings set("settings.conf");
    QFile::remove(set.fileName());

    {
        KDESettings set("settings.conf");
        QCOMPARE(set.allKeys().join(","), QString(""));
    }

    {
        KDESettings set("settings.conf");
        QCOMPARE(set.value("test","empty").toString(),QString("empty"));
    }
    {
        KDESettings set("settings.conf");
        set.setValue("test", "value");
    }
    {
        KDESettings set("settings.conf");
        QCOMPARE(set.allKeys().join(","), QString("test"));
        QCOMPARE(set.value("test").toString(), QString("value"));
    }

    {
        KDESettings set("settings.conf");
        set.beginGroup("group");
        QCOMPARE(set.value("test","empty").toString(),QString("empty"));
    }
    {
        KDESettings set("settings.conf");
        set.beginGroup("group");
        set.setValue("test","group_value");
    }
    {
        KDESettings set("settings.conf");
        QCOMPARE(set.contains("group"),false);
    }
    {
        KDESettings set("settings.conf");
        QCOMPARE(set.childGroups().join(","),QString("group"));
    }

    {
        KDESettings set("settings.conf");
        QCOMPARE( set.value("test","empty").toString(), QString("value") );
    }
    {
        KDESettings set("settings.conf");
        set.beginGroup("group");
        QCOMPARE( set.value("test","empty").toString(), QString("group_value") );
    }

    {
        KDESettings set("settings.conf");
        QCOMPARE(set.allKeys().join(","),QString("group/test,test"));
    }

    {
        KDESettings set("settings.conf");
        set.remove("test");
    }
    {
        KDESettings set("settings.conf");
        set.beginGroup("group");
        set.remove("test");
    }
    {
        KDESettings set("settings.conf");
        QCOMPARE(set.value("test","empty").toString(),QString("empty"));
    }
    {
        KDESettings set("settings.conf");
        set.beginGroup("group");
        QCOMPARE(set.value("test","empty").toString(),QString("empty"));
    }
    {
        KDESettings set("settings.conf");
        QCOMPARE(set.allKeys().join(","),QString(""));
    }

    {
        KDESettings set("settings.conf");
        set.setValue("bla", "bla");
        set.setValue("stringlist", QStringList() << "Hello" << "; " << "World!");
    }
    {
        KDESettings set("settings.conf");
        QCOMPARE(set.value("bla").toString(), QString("bla"));
        QCOMPARE(set.value("stringlist").toStringList(), QStringList() << "Hello" << "; " << "World!");
    }
}

void TestSettings::testArray()
{

    {
        KDESettings set("settings.conf");
        set.beginWriteArray("Array");
        for (int i = 0; i < 10; i++) {
            set.setArrayIndex(i);
            set.setValue("key", QString("value %1").arg(i));
        }
        set.endArray();
    }
    {
        KDESettings set("settings.conf");
        int count = set.beginReadArray("Array");
        for (int i = 0; i < count; i++) {
            set.setArrayIndex(i);
            QCOMPARE(set.value("key").toString(), QString("value %1").arg(i));
        }
        set.endArray();
    }

}

void TestSettings::testBug158()
{
    {
        KDESettings set("settings.conf");
        QByteArray buf;
        buf.append('\\');
        buf.append('\"');
        set.setValue("buf",buf);
    }
    {
        KDESettings set("settings.conf");
        QByteArray buf=set.value("buf").toByteArray();
        QCOMPARE(buf.count(),2);
        QCOMPARE(buf.at(0),'\\');
        QCOMPARE(buf.at(1),'\"');
    }
}

QTEST_APPLESS_MAIN(TestSettings)

#include "tst_kdesettings.moc"
