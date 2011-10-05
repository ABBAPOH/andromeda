#include "pluginspec.h"
#include "pluginspec_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QPluginLoader>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

#include "pluginmanager.h"

//#define DEBUG_OUTPUT

using namespace ExtensionSystem;

Version::Version() :
    build(0),
    revision(0)
{
    this->major = 0;
    this->minor = 0;
}

Version::Version(const QString &version) :
    build(0),
    revision(0)
{
    this->major = 0;
    this->minor = 0;

    // mathes to Major.Minor.Patch.Fix version
    static QRegExp regExp = QRegExp("([0-9]+)(?:\\.([0-9]+))?(?:\\.([0-9]+))?(?:\\.([0-9]+))?");
    if ( regExp.exactMatch(version) ) {
        major = regExp.cap(1).toInt();
        minor = regExp.cap(2).toInt();
        build = regExp.cap(3).toInt();
        revision = regExp.cap(4).toInt();
    }
}

QString Version::toString() const
{
    return QString("%1.%2.%3.%4").arg(major).arg(minor).arg(build).arg(revision);
}

Version Version::fromString(const QString &version)
{
    return Version(version);
}

bool Version::operator==(const Version &other) const
{
    return major == other.major && minor == other.minor && build == other.build && revision == other.revision;
}

PluginSpecPrivate::PluginSpecPrivate(PluginSpec *qq) :
    q_ptr(qq),
    plugin(0),
    loader(new QPluginLoader(q_ptr)),
    loaded(false),
    loadOnStartup(true),
    hasError(false)
{
    errorString = "Unknown Error";
}

bool PluginSpecPrivate::load()
{
#ifdef DEBUG_OUTPUT
    qDebug() << "loading" << name << version.toString();
    qDebug() << "  resolving dependencies for" << name;
#endif
    if (!resolveDependencies())
        return false;

    bool ok = true;

    QString tmp("Can't load plugin: %1");
    QStringList errorMessages;
    foreach (PluginSpec *spec, dependencySpecs) {
        spec->load();
        if (!spec->loaded()) {
            ok = false;
            errorMessages << tmp.arg(spec->name());
        }
    }

    if (!ok) {
        setError(errorMessages.join("\n"));
        return false;
    }

#ifdef DEBUG_OUTPUT
    qDebug() << "    loading library" << QFileInfo(libraryPath).fileName();
#endif
    if (!loadLibrary())
        return false;

#ifdef DEBUG_OUTPUT
    qDebug() << "      initializing" << name;
#endif
    if (!plugin->initialize()) {
        setError("Failed to initialize plugin");
        return false;
    }
#ifdef DEBUG_OUTPUT
    qDebug() << "======";
#endif

    return true;
}

bool PluginSpecPrivate::loadLibrary()
{
    if (plugin)
        return true;

    QObject * object = loader->instance();
    if (!object) {
        setError(QObject::tr("Can't load plugin: ", "PluginSpec") + loader->errorString());
        return false;
    }

    IPlugin *plugin = qobject_cast<IPlugin *>(object);
    if (!plugin) {
        setError(QObject::tr("Can't load plugin: not a valid plugin", "PluginSpec"));
        return false;
    }

    this->plugin = plugin;
    return true;
}

bool PluginSpecPrivate::unload()
{
    bool ok = true;
    QString errorMessage;

#ifdef DEBUG_OUTPUT
    qDebug() << "unloading" << name << version.toString();
    qDebug() << "  unloading dependencies for" << name;
#endif
    foreach (PluginSpec *spec, dependentSpecs) {
        spec->unload();
        if (spec->loaded()) {
            ok = false;
            errorMessage += "Can't unload plugin: " + spec->name() + " is not unloaded";
        }
    }

    if (!ok) {
        setError(errorMessage);
        return false;
    }

#ifdef DEBUG_OUTPUT
    qDebug() << "    shutting down" << name;
#endif
    plugin->shutdown();

#ifdef DEBUG_OUTPUT
    qDebug() << "      unloading library" << QFileInfo(libraryPath).fileName();
#endif
    if (!unloadLibrary())
        return false;
#ifdef DEBUG_OUTPUT
    qDebug() << "======";
#endif

    return true;
}

bool PluginSpecPrivate::unloadLibrary()
{
    if (!loader->unload()) {
        setError("Can't unload plugin library: " + loader->errorString());
        return false;
    }
    plugin = 0;
    return true;
}

//#warning TODO: check circular dependencies
bool PluginSpecPrivate::resolveDependencies()
{
    Q_Q(PluginSpec);

    QList<PluginSpec*> specs = PluginManager::instance()->plugins();
    QList<PluginSpec*> resolvedSpecs;

    PluginSpec * dependencySpec = 0;
    bool ok = true;
    QString errorMessage;

    foreach (PluginDependency dep, dependencies) {
        dependencySpec = 0;
        foreach (PluginSpec *spec, specs) {
            if (spec->provides(dep)) {
                dependencySpec = spec;
                break;
            }
        }
        if (dependencySpec == 0) {
            ok = false;
            errorMessage.append(QObject::tr("PluginSpec", "Can't resolve dependency '%1(%2)'")
                                .arg(dep.name()).arg(dep.version().toString()));
        } else {
            resolvedSpecs.append(dependencySpec);
        }
    }

    if (!ok) {
        setError(errorMessage);
        return false;
    }

    foreach (PluginSpec *spec, resolvedSpecs) {
        if (!spec->d_ptr->dependentSpecs.contains(q))
            spec->d_ptr->dependentSpecs.append(q);
    }

    dependencySpecs = resolvedSpecs;
    return true;
}

int PluginSpecPrivate::compareVersion(const Version &version1, const Version &version2)
{
    if (version1.major < version2.major)
        return -1;
    if (version1.major > version2.major)
        return 1;

    if (version1.minor < version2.minor)
        return -1;
    if (version1.minor > version2.minor)
        return 1;

    if (version1.build < version2.build)
        return -1;
    if (version1.build > version2.build)
        return 1;

    if (version1.revision < version2.revision)
        return -1;
    if (version1.revision > version2.revision)
        return 1;

    return 0;
}

QString PluginSpecPrivate::getLibraryPath(const QString &path)
{
    QFileInfo info(path);
    QString baseName = info.baseName();
    QString absolutePath = info.absolutePath();
#ifdef Q_OS_WIN
    return absolutePath + "/" + "lib" + baseName + ".dll";
#endif
#ifdef Q_OS_MAC
    return absolutePath + "/" + "lib" + baseName + ".dylib";
#else
#ifdef Q_OS_UNIX
    return absolutePath + "/" + "lib" + baseName + ".so";
#endif
#endif
}

void PluginSpecPrivate::setError(const QString &message)
{
    qWarning() << "PluginSpec error:" << message;
    hasError = true;
    errorString = message;
    emit q_ptr->error(message);
}

bool PluginSpecPrivate::readTextFormat(const QString &path)
{
    QSettings specFile(path, QSettings::IniFormat);

    name = specFile.value("name").toString();
    version = Version::fromString(specFile.value("version").toString());
    compatibilityVersion = Version::fromString(specFile.value("compatibilityVersion").toString());
    vendor = specFile.value("vendor").toString();
    category = specFile.value("category").toString();
    copyright = specFile.value("copyright").toString();
    license = specFile.value("license").toString();
    description = specFile.value("description").toString();
    url = specFile.value("url").toString();

    specFile.beginGroup("Dependencies");
    foreach (QString key, specFile.childKeys()) {
        if (key.length() == 1 && key.at(0).isDigit()) {
            QStringList list = specFile.value(key).toStringList();
            if (list.size() == 2) {
                dependencies.append(PluginDependency(list.at(0), list.at(1)));
            }
        }
    }
    specFile.endGroup();

    return true;
}

bool PluginSpecPrivate::writeTextFormat(const QString &path)
{
    QSettings specFile(path, QSettings::IniFormat);

    specFile.setValue(QLatin1String("name"), name);
    specFile.setValue(QLatin1String("version"), version.toString());
    specFile.setValue(QLatin1String("compatibilityVersion"), compatibilityVersion.toString());
    specFile.setValue(QLatin1String("vendor"), vendor);
    specFile.setValue(QLatin1String("category"), category);
    specFile.setValue(QLatin1String("copyright"), copyright);
    specFile.setValue(QLatin1String("license"), license);
    specFile.setValue(QLatin1String("description"), description);
    specFile.setValue(QLatin1String("url"), url);

    specFile.beginGroup("Dependencies");
    int counter = 0;
    foreach (const PluginDependency &dependency, dependencies) {
        QStringList l;
        l << dependency.name() << dependency.version().toString();
        specFile.setValue(QString::number(counter++), l);
    }
    specFile.endGroup();

    return true;
}

static void readXmlPluginSpecDependencies(PluginSpecPrivate *d, QXmlStreamReader &reader)
{
    QString name;
    bool readingElement = false;
    while (!reader.atEnd()) {
        reader.readNext();
        name = reader.name().toString();
        switch (reader.tokenType()) {

        case QXmlStreamReader::StartElement:

            if (readingElement)
                reader.raiseError(QObject::tr("Unexpected token '%1'").arg(name));
            readingElement = true;

            if (name == QLatin1String("dependency")) {
                QString depName = reader.attributes().value(QLatin1String("name")).toString();
                QString depVersion = reader.attributes().value(QLatin1String("version")).toString();
                d->dependencies.append(PluginDependency(depName, depVersion));
            } else {
                reader.raiseError(QObject::tr("Unknown element '%1'").arg(name));
            }

            break;

        case QXmlStreamReader::EndElement:

            readingElement = false;

            if (name == QLatin1String("dependency"))
                break;

            if (name == QLatin1String("dependencyList"))
                return;

            reader.raiseError(QObject::tr("Expected </dependencyList>"));
            break;

        case QXmlStreamReader::Comment:
            break;

        case QXmlStreamReader::Characters:

            if (!reader.text().toString().trimmed().isEmpty())
                reader.raiseError(QObject::tr("Unexpected character sequence"));
            break;

        default:
            reader.raiseError(QObject::tr("Unexpected token"));
            break;
        }
    }
}

static void readXmlPluginSpecOption(PluginSpecPrivate *d, QXmlStreamReader &reader, Option &option)
{
    QString name;
    bool readingElement = false;
    while (!reader.atEnd()) {
        reader.readNext();
        name = reader.name().toString();
        switch (reader.tokenType()) {

        case QXmlStreamReader::StartElement:

            if (readingElement)
                reader.raiseError(QObject::tr("Unexpected token '%1'").arg(name));
            readingElement = true;

            if (name == QLatin1String("value")) {

                // TODO: check attributes
                QXmlStreamAttributes attrs = reader.attributes();
                QString type = attrs.value(QLatin1String("type")).toString();
                QString name = attrs.value(QLatin1String("name")).toString();
                option.addValue((Options::Type)QVariant::nameToType(type.toLatin1()), name);
                option.setSingle(false);

            } else if (name == QLatin1String("description")) {

                option.setDescription(reader.readElementText());
                readingElement = false;

            } else {
                reader.raiseError(QObject::tr("Unknown element '%1'").arg(name));
            }

            break;

        case QXmlStreamReader::EndElement:

            readingElement = false;

            if (name == QLatin1String("value"))
                break;

            if (name == QLatin1String("option"))
                return;

            reader.raiseError(QObject::tr("Expected </option>"));
            break;

        case QXmlStreamReader::Comment:
            break;

        case QXmlStreamReader::Characters:

            if (!reader.text().toString().trimmed().isEmpty())
                reader.raiseError(QObject::tr("Unexpected character sequence"));
            break;

        default:
            reader.raiseError(QObject::tr("Unexpected token"));
            break;
        }
    }
}

bool stringToBool(bool *ok, const QString &s);

static void readXmlPluginSpecOptions(PluginSpecPrivate *d, QXmlStreamReader &reader)
{
    QString name;
    bool readingElement = false;
    while (!reader.atEnd()) {
        reader.readNext();
        name = reader.name().toString();
        switch (reader.tokenType()) {

        case QXmlStreamReader::StartElement:

            if (readingElement)
                reader.raiseError(QObject::tr("Unexpected token '%1'").arg(name));
            readingElement = true;

            if (name == QLatin1String("option")) {

                QXmlStreamAttributes attrs = reader.attributes();
                QString name = attrs.value(QLatin1String("name")).toString();
                QString shortName = attrs.value(QLatin1String("shortName")).toString();
                QString type = attrs.value(QLatin1String("type")).toString();
                QString multiple = attrs.value(QLatin1String("multiple")).toString();

                Option opt(name);
                opt.setSingle(true);
                opt.setMultiple(stringToBool(0, multiple));
                opt.setShortName(shortName.isEmpty() ? QChar() : shortName[0]);

                readXmlPluginSpecOption(d, reader, opt);
                if (opt.isSingle()) {
                    opt.addValue((Options::Type)QVariant::nameToType(type.toLatin1()), QString());
                }
                d->options.append(opt);
            } else {
                reader.raiseError(QObject::tr("Unknown element '%1'").arg(name));
            }

            break;

        case QXmlStreamReader::EndElement:

            readingElement = false;

//            if (name == QLatin1String("option"))
//                break;

            if (name == QLatin1String("options"))
                return;

            reader.raiseError(QObject::tr("Expected </options>"));
            break;

        case QXmlStreamReader::Comment:
            break;

        case QXmlStreamReader::Characters:

            if (!reader.text().toString().trimmed().isEmpty())
                reader.raiseError(QObject::tr("Unexpected character sequence"));
            break;

        default:
            reader.raiseError(QObject::tr("Unexpected token"));
            break;
        }
    }
}

static void readXmlPluginSpecAttributes(PluginSpecPrivate *d, QXmlStreamReader &reader)
{
    QString name;
    while (!reader.atEnd()) {
        reader.readNext();
        name = reader.name().toString();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:

            if (name == QLatin1String("vendor")) {
                d->vendor = reader.readElementText();
            } else if (name == QLatin1String("copyright")) {
                d->copyright = reader.readElementText();
            } else if (name == QLatin1String("license")) {
                d->license = reader.readElementText();
            } else if (name == QLatin1String("category")) {
                d->category = reader.readElementText();
            } else if (name == QLatin1String("description")) {
                d->description = reader.readElementText();
            } else if (name == QLatin1String("url")) {
                d->url = reader.readElementText();
            } else if (name == QLatin1String("dependencyList")) {
                readXmlPluginSpecDependencies(d, reader);
            } else if (name == QLatin1String("options")) {
                readXmlPluginSpecOptions(d, reader);
            } else {
                reader.raiseError(QObject::tr("Unknown element %1").arg(name));
            }

            break;
        case QXmlStreamReader::EndElement:

            if (name == QLatin1String("plugin"))
                return;

            reader.raiseError(QObject::tr("Expected </plugin>"));
            break;

        case QXmlStreamReader::Comment:
            break;

        case QXmlStreamReader::Characters:

            if (!reader.text().toString().trimmed().isEmpty())
                reader.raiseError(QObject::tr("Unexpected character sequence"));
            break;

        default:
            reader.raiseError(QObject::tr("Unexpected token"));
            break;
        }
    }
}

static void readXmlPluginSpec(PluginSpecPrivate *d, QXmlStreamReader &reader)
{
    if (reader.atEnd()) {
        reader.raiseError("Empty xml file");
        return;
    }

    QString name;
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:

            name = reader.name().toString();
            if (name == QLatin1String("plugin")) {

                d->name = reader.attributes().value(QLatin1String("name")).toString();
                d->version = Version(reader.attributes().value(QLatin1String("version")).toString());
                d->compatibilityVersion = Version(reader.attributes().value(QLatin1String("compatVersion")).toString());
                readXmlPluginSpecAttributes(d, reader);

            } else {
                reader.raiseError(QObject::tr("Unknown element %1").arg(name));
            }

            break;

        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::Characters:
            break;

        case QXmlStreamReader::EndElement:

            reader.raiseError(QObject::tr("Expected end of document"));
            break;

        case QXmlStreamReader::EndDocument:
            return;

        default:
            qDebug() << reader.tokenString();
            reader.raiseError(QObject::tr("Unexpected token"));
            break;
        }
    }
}

bool PluginSpecPrivate::readXmlFormat(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Can't open file" << path << "for reading.";
        return false;
    }

    QXmlStreamReader reader(&file);

    readXmlPluginSpec(this, reader);

    if (reader.hasError()) {
        qWarning() << (QObject::tr("Error parsing file %1: %2, at line %3, column %4")
                       .arg(file.fileName())
                       .arg(reader.errorString())
                       .arg(reader.lineNumber())
                       .arg(reader.columnNumber()));
        return false;
    }
    return true;
}

bool PluginSpecPrivate::writeXmlFormat(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "Can't open file" << path << "for writing.";
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartElement(QLatin1String("plugin"));
    writer.writeAttribute(QLatin1String("name"), name);
    writer.writeAttribute(QLatin1String("version"), version.toString());
    writer.writeAttribute(QLatin1String("compatVersion"), compatibilityVersion.toString());

    writer.writeTextElement(QLatin1String("vendor"), vendor);
    writer.writeTextElement(QLatin1String("copyright"), copyright);
    writer.writeTextElement(QLatin1String("license"), license);
    writer.writeTextElement(QLatin1String("category"), category);
    writer.writeTextElement(QLatin1String("description"), description);
    writer.writeTextElement(QLatin1String("url"), url);

    if (!dependencies.isEmpty()) {
        writer.writeStartElement(QLatin1String("dependencyList"));

        foreach (const PluginDependency &dependency, dependencies) {
            writer.writeStartElement(QLatin1String("dependency"));
            writer.writeAttribute(QLatin1String("name"), dependency.name());
            writer.writeAttribute(QLatin1String("version"), dependency.version().toString());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }

    writer.writeEndElement();

    return true;
}

namespace ExtensionSystem {

QDataStream & operator<<(QDataStream &s, const Version &version)
{
    s << (quint32)version.major;
    s << (quint32)version.minor;
    s << (quint32)version.build;
    s << (quint32)version.revision;
    return s;
}

QDataStream & operator>>(QDataStream &s, Version &version)
{
    quint32 tmp;
    s >> tmp; version.major = tmp;
    s >> tmp; version.minor = tmp;
    s >> tmp; version.build = tmp;
    s >> tmp; version.revision = tmp;
    return s;
}

QDataStream & operator>>(QDataStream &s, PluginDependency &dependency)
{
    QString name;
    Version version;
    s >> name;
    s >> version;
    dependency = PluginDependency(name, version);
    return s;
}

QDataStream & operator<<(QDataStream &s, const PluginDependency &dependency)
{
    s << dependency.name();
    s << dependency.version();
    return s;
}

QDataStream & operator>>(QDataStream &s, PluginSpecPrivate &pluginSpec)
{
    PluginSpecPrivate * d = &pluginSpec;
    s.device()->read(4);
    quint32 version;
    s >> version;

    s >> d->name;
    s >> d->version;
    s >> d->compatibilityVersion;
    s >> d->vendor;
    s >> d->category;
    s >> d->copyright;
    s >> d->license;
    s >> d->description;
    s >> d->url;
    s >> d->dependencies;
    return s;
}

QDataStream & operator<<(QDataStream &s, const PluginSpecPrivate &pluginSpec)
{
    const PluginSpecPrivate * d = &pluginSpec;
    s << (uchar)'S' << (uchar)'P' << (uchar)'E' << (uchar)'C';
    quint32 version = 1;
    s << version;
    s << d->name;
    s << d->version;
    s << d->compatibilityVersion;
    s << d->vendor;
    s << d->category;
    s << d->copyright;
    s << d->license;
    s << d->description;
    s << d->url;
    s << d->dependencies;
    return s;
}

} // namespace ExtensionSystem

/*!
    \fn PluginDependency::PluginDependency(const QString &name, const QString &version)
    \brief Constructs PluginDependency class with given \a name and \a version.
*/
PluginDependency::PluginDependency(const QString &name, const QString &version)
{
    m_name = name;
    m_version = Version(version);
}

PluginDependency::PluginDependency(const QString &name, const Version &version)
{
    m_name = name;
    m_version = version;
}

/*!
    \fn bool PluginDependency::operator==(const PluginDependency &other)
    \brief Comparator for PluginDependency.
*/
bool PluginDependency::operator==(const PluginDependency &other)
{
    return m_name == other.m_name && m_version == other.m_version;
}

/*!
    \fn PluginSpec::PluginSpec()
    \internal
*/
PluginSpec::PluginSpec(QObject *parent) :
    QObject(parent),
    d_ptr(new PluginSpecPrivate(this))
{
}

/*!
    \fn PluginSpec::~PluginSpec()
    \internal
*/
PluginSpec::~PluginSpec()
{
    QSettings s;
    s.beginGroup(name());
    s.setValue("loadOnStartup", loadOnStartup());
    s.endGroup();

    delete d_ptr;
}

/*!
    \fn PluginSpec::PluginSpec(const QString & path)
    \internal
    Constructs PluginSpec using data received from file located at \a path.
*/
PluginSpec::PluginSpec(const QString & path) :
    d_ptr(new PluginSpecPrivate(this))
{
    read(path);
}

/*!
    \fn QString PluginSpec::name() const
    \brief Returns name of the plugin.
    Name and version are used to resolve dependencies.
*/
QString PluginSpec::name() const
{
    return d_func()->name;
}

/*!
    \fn Version PluginSpec::version() const
    \brief Returns version of the plugin.
    Version should be in format "Major.Minor.Build.Revision".
*/
Version PluginSpec::version() const
{
    return d_func()->version;
}

/*!
    \fn Version PluginSpec::compatibilityVersion() const
    \brief Returns compatibility version of the plugin.
*/
Version PluginSpec::compatibilityVersion() const
{
    return d_func()->compatibilityVersion;
}

/*!
    \fn QString PluginSpec::vendor() const
    \brief Returns vendor of the plugin.
*/
QString PluginSpec::vendor() const
{
    return d_func()->vendor;
}

/*!
    \fn QString PluginSpec::category() const
    \brief Returns category (i.e. group) that plugin belongs to.
*/
QString PluginSpec::category() const
{
    return d_func()->category;
}

/*!
    \fn QString PluginSpec::copyright() const
    \brief Returns copyright.
*/
QString PluginSpec::copyright() const
{
    return d_func()->copyright;
}

/*!
    \fn QString PluginSpec::license() const
    \brief Returns full licence of the plugin.
*/
QString PluginSpec::license() const
{
    return d_func()->license;
}

/*!
    \fn QString PluginSpec::description() const
    \brief Returns description of the plugin.
*/
QString PluginSpec::description() const
{
    return d_func()->description;
}

/*!
    \fn QString PluginSpec::url() const
    \brief Returns vendor's site url.
*/
QString PluginSpec::url() const
{
    return d_func()->url;
}

/*!
    \fn QList<PluginDependency> PluginSpec::dependencies() const
    \brief Returns list of dependencies that are needed for this plugin.
*/
QList<PluginDependency> PluginSpec::dependencies() const
{
    return d_func()->dependencies;
}

/*!
    \fn QList<PluginSpec*> PluginSpec::dependencySpecs() const
    \brief Returns list of PluginSpecs that need for this plugin.
*/
QList<PluginSpec*> PluginSpec::dependencySpecs() const
{
    return d_func()->dependencySpecs;
}

/*!
    \fn QList<PluginSpec*> PluginSpec::dependentSpecs() const
    \brief Returns list of PluginSpecs that require this plugin.
*/
QList<PluginSpec*> PluginSpec::dependentSpecs() const
{
    return d_func()->dependentSpecs;
}

/*!
    \fn QString PluginSpec::libraryPath() const
    \brief Returns path to dynamic library.
    In case of static plugin, returns empty string.

    \sa PluginSpec::isStatic
*/
QString PluginSpec::libraryPath() const
{
    return d_func()->libraryPath;
}

/*!
    \fn void PluginSpec::load()
    \brief Loads plugin into memory and initialises it.
*/
void PluginSpec::load()
{
    Q_D(PluginSpec);

    if (d->loaded)
        return;

    if (d->load()) {
        d->loaded = true;
        emit loadedChanged(true);
    }
}

/*!
    \fn void PluginSpec::unload()
    \brief UnLoads plugin from memory and shutdowns it.
*/
void PluginSpec::unload()
{
    Q_D(PluginSpec);

    if (!d->loaded)
        return;

    if (d->unload()) {
        d->loaded = false;
        emit loadedChanged(false);
    }
}

/*!
    \fn void PluginSpec::setEnabled(bool enabled)
    \brief Enables or disables plugin.
    Note that library is loaded or unloaded when necessary.
*/
void PluginSpec::setLoaded(bool yes)
{
    if (yes)
        load();
    else
        unload();
}

/*!
    \fn bool PluginSpec::loaded() const
    \brief Returns if plugin is loaded.
*/
bool PluginSpec::loaded() const
{
    return d_func()->loaded;
}

void PluginSpec::setLoadOnStartup(bool yes)
{
    Q_D(PluginSpec);

    if (d->loadOnStartup == yes)
        return;

    d->loadOnStartup = yes;
    emit loadOnStartupChanged(yes);
}

bool PluginSpec::loadOnStartup() const
{
    return d_func()->loadOnStartup;
}

bool PluginSpec::canBeUnloaded() const
{
    foreach (PluginSpec *spec, dependentSpecs()) {
        if (spec->loaded())
            return false;
    }
    return true;
}

/*!
    \fn QString PluginSpec::errorString()
    \brief Returns message of last occured error.
*/
QString PluginSpec::errorString() const
{
    return d_func()->errorString;
}

/*!
    \fn bool PluginSpec::hasError()
    \brief Returns true if error occured.
*/
bool PluginSpec::hasError() const
{
    return d_func()->hasError;
}

/*!
    \fn IPlugin *PluginSpec::plugin() const
    \brief Returnes pointer to IPlugin implementation.
*/
IPlugin *PluginSpec::plugin() const
{
    return d_func()->plugin;
}

/*!
    \fn bool PluginSpec::provides(const PluginDependency &pluginDependency) const
    Returns if this plugin can be used to fill in a of the given \a dependency.
    \sa PluginSpec::dependencies()
*/
bool PluginSpec::provides(const PluginDependency &dependency) const
{
    if (QString::compare(dependency.name(), name(), Qt::CaseInsensitive) != 0)
         return false;
    return (PluginSpecPrivate::compareVersion(version(), dependency.version()) >= 0) &&
           (PluginSpecPrivate::compareVersion(compatibilityVersion(), dependency.version()) <= 0);
}

bool PluginSpec::read(const QString &path)
{
    Q_D(PluginSpec);

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    if (file.peek(4) == QByteArray("SPEC")) {
        QDataStream s(&file);
        s.setByteOrder(QDataStream::BigEndian);
        s >> *this->d_func();
    } else if (file.peek(10).contains("<plugin")) {
        if (!d->readXmlFormat(path))
            return false;
    } else {
        file.close();

        if (!d->readTextFormat(path))
            return false;
    }

    d->libraryPath = d->getLibraryPath(path);
    d->loader->setFileName(d->libraryPath);

    QSettings s;
    s.beginGroup(name());
    if (s.contains("loadOnStartup"))
        d->loadOnStartup = s.value("loadOnStartup").toBool();
    s.endGroup();

    return true;
}

bool PluginSpec::write(const QString &path, Format format)
{
    if (format == BinaryFormat) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
            return false;

        QDataStream s(&file);
        s.setByteOrder(QDataStream::BigEndian);

        s << *this->d_func();
    } else if (format == XmlFormat) {
        return d_func()->writeXmlFormat(path);
    } else if (format == TextFormat) {
        return d_func()->writeTextFormat(path);
    }

    return true;
}
