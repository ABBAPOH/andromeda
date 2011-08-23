#include <QtCore/QString>
#include <QtTest/QtTest>

#include <pluginspec.h>
#include <mutablepluginspec.h>

using namespace ExtensionSystem;

class PluginSpecTest : public QObject
{
    Q_OBJECT

public:
    PluginSpecTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSerialise();

private:
    void compareSpecs(PluginSpec *first, PluginSpec *second);
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
    MutablePluginSpec spec;
    spec.setName(QLatin1String("Name"));
    spec.setVersion(Version::fromString(QLatin1String("1.2.3.4")));
    spec.setCompatibilityVersion(Version::fromString(QLatin1String("1.2.3.4")));
    spec.setVendor(QLatin1String("Vendor"));
    spec.setCategory(QLatin1String("Category"));
    spec.setCopyright(QLatin1String("Copyright"));
    spec.setLicense(QLatin1String("License"));
    spec.setDescription(QLatin1String("Description"));
    spec.setUrl(QLatin1String("Url"));
    spec.setDependencies(QList<PluginDependency>() << PluginDependency(QLatin1String("Name"), QLatin1String("1.2.3.4")));

    spec.write(QLatin1String("File.pluginspec"));

    PluginSpec spec2;
    spec2.read(QLatin1String("File.pluginspec"));

    compareSpecs(&spec, &spec2);
}

void PluginSpecTest::compareSpecs(PluginSpec *first, PluginSpec *second)
{
    QCOMPARE(first->name(), second->name());
    QCOMPARE(first->version(), second->version());
    QCOMPARE(first->compatibilityVersion(), second->compatibilityVersion());
    QCOMPARE(first->vendor(), second->vendor());
    QCOMPARE(first->category(), second->category());
    QCOMPARE(first->copyright(), second->copyright());
    QCOMPARE(first->license(), second->license());
    QCOMPARE(first->description(), second->description());
    QCOMPARE(first->url(), second->url());
    QCOMPARE(first->dependencies(), second->dependencies());
}

QTEST_APPLESS_MAIN(PluginSpecTest)

#include "tst_pluginspec.moc"
