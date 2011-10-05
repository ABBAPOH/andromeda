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
    void testSerialiseText();
    void testSerialiseXml();
    void testSerialiseBinary();
    void benchmarkText();
    void benchmarkXml();
    void benchmarkBinary();

private:
    void compareSpecs(PluginSpec *first, PluginSpec *second);

private:
    MutablePluginSpec spec;
};

PluginSpecTest::PluginSpecTest()
{
}

void PluginSpecTest::initTestCase()
{
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
}

void PluginSpecTest::cleanupTestCase()
{
}

void PluginSpecTest::testSerialiseText()
{
    spec.write(QLatin1String("Text.pluginspec"), PluginSpec::TextFormat);

    PluginSpec spec2;
    spec2.read(QLatin1String("Text.pluginspec"));

    compareSpecs(&spec, &spec2);
}

void PluginSpecTest::testSerialiseXml()
{
    spec.write(QLatin1String("Xml.pluginspec"), PluginSpec::XmlFormat);

    PluginSpec spec2;
    spec2.read(QLatin1String("Xml.pluginspec"));

    compareSpecs(&spec, &spec2);
}

void PluginSpecTest::testSerialiseBinary()
{
    spec.write(QLatin1String("Binary.pluginspec"), PluginSpec::BinaryFormat);

    PluginSpec spec2;
    spec2.read(QLatin1String("Binary.pluginspec"));

    compareSpecs(&spec, &spec2);
}

void PluginSpecTest::benchmarkText()
{
    PluginSpec spec2;

    QBENCHMARK {
        spec.write(QLatin1String("Text.pluginspec"), PluginSpec::TextFormat);

        spec2.read(QLatin1String("Text.pluginspec"));
    }
}

void PluginSpecTest::benchmarkXml()
{
    PluginSpec spec2;

    QBENCHMARK {
        spec.write(QLatin1String("Xml.pluginspec"), PluginSpec::XmlFormat);

        spec2.read(QLatin1String("Xml.pluginspec"));
    }
}

void PluginSpecTest::benchmarkBinary()
{
    PluginSpec spec2;

    QBENCHMARK {
        spec.write(QLatin1String("Binary.pluginspec"), PluginSpec::BinaryFormat);

        spec2.read(QLatin1String("Binary.pluginspec"));
    }
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
