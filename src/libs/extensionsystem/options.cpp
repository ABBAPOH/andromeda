#include "options.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>

namespace ExtensionSystem {

class OptionData : public QSharedData
{
public:
    OptionData() : multiple(false), single(false) {}
    OptionData(const OptionData &o) :
        name(o.name),
        shortName(o.shortName),
        description(o.description),
        values(o.values),
        multiple(o.multiple),
        single(o.single)
    {}

    QString name;
    QChar shortName;
    QString description;
    QList< QPair<Options::Type, QString> > values;
    bool multiple;
    bool single;
};

} // namespace ExtensionSystem

using namespace ExtensionSystem;

Options::Options()
{
    QString s;
    Option d(s);
    addOption(d);
}

QList<Option> Options::options() const
{
    return nameToOption.values();
}

bool Options::addOption(const QChar shortName, const QString &name, Options::Type type, const QString &description)
{
    Option opt(shortName, name, description);
    opt.addValue(type, QString());
    opt.setSingle(true);
    return addOption(opt);
}

bool Options::addOption(const QString &name, Options::Type type, const QString &description)
{
    Option opt(name, description);
    opt.addValue(type, QString());
    opt.setSingle(true);
    return addOption(opt);
}

bool Options::addOption(const Option &option)
{
    QString name = option.name();
    QChar c = option.shortName();

    m_errorString.clear();

    if (nameToOption.contains(name)) {
        m_errorString = QObject::tr("Already have option with name %1").arg(name);
        return false;
    }

    if (shortToName.contains(c)) {
        m_errorString = QObject::tr("Already have option with short name %1").arg(c);
        return false;
    }

    QSet<QString> valueNames;
    for (int i = 0; i < option.count(); i++) {
        valueNames.insert(option.name(i));
    }

    if (valueNames.count() < option.count()) {
        m_errorString = QObject::tr("Option %1 has duplicate value names").arg(name);
        return false;
    }

    nameToOption.insert(name, option);

    if (!c.isNull())
        shortToName.insert(c, name);

    return true;
}

void Options::clear()
{
    m_values.clear();
}

QString Options::defaultOption() const
{
    return m_defaultOption;
}

void Options::setDefaultOption(const QString &name)
{
    m_defaultOption = name;
}

QString Options::errorString() const
{
    return (!m_errorString.isEmpty()) ? m_errorString : QObject::tr("No error");
}

bool stringToBool(bool *ok, const QString &s)
{
    QString lower = s.toLower();
    if (ok)
        *ok = true;

    if (lower == "false" || lower == "no" || lower == "off" || lower == "0")
        return false;
    if (lower == "true" || lower == "yes" || lower == "on" || lower == "1")
        return true;

    if (ok)
        *ok = false;
    return false;
}

QChar stringToChar(bool *ok, const QString &s)
{
    if (s.size() == 1) {
        if (ok)
            *ok = true;
        return s.at(0);
    }

    if (ok)
        *ok = false;
    return QChar();
}

// FIXME: default&multiple option with empty argument list
bool Options::parse(const QStringList &lst)
{
    QStringList arguments = lst;
    int argi = 0;
    Option opt = nameToOption.value(m_defaultOption, Option());
    int argRead = 0;
    QVariant singleValue;
    QVariantMap multiValue;
    QVariantList list;

    m_errorString.clear();

    while (++argi < arguments.count()) {
        QString arg = arguments[argi];
        QVariant value;

        if (arg == QLatin1String("--")) {
            arg += m_defaultOption;
        }

        if (arg.startsWith(QLatin1String("--"))) {
            if (argi > 1 && (opt.multiple() || !opt.multiple() && argRead < opt.count()) && opt.name() != m_defaultOption) {
                if (argRead >= opt.count()) {
                    QString name = opt.name();
                    if (m_values.contains(name)) {
                        m_errorString = QObject::tr("Redeclaration of option %1").arg(name);
                        return false;
                    }
                    m_values.insert(opt.name(), opt.d->single ? singleValue : QVariant(multiValue));
                    singleValue.clear();
                    multiValue.clear();
                    list.clear();
                    opt = nameToOption.value(m_defaultOption, Option());
                } else {
                    m_errorString = QObject::tr("Not enough arguments for option %1").arg(opt.name());
                    return false;
                }
            }

            arg = arg.mid(2);

            if (!nameToOption.contains(arg)) {
                m_errorString = QObject::tr("Unknown option %1").arg(arg);
                return false;
            }

            opt = nameToOption.value(arg, Option());
            argRead = 0;
            continue;
        }

        if (arg.startsWith(QLatin1Char('-'))) {
            for (int i = arg.length() - 1; i >= 1; i--) {
                QString name = shortToName.value(arg[i]);
                if (!name.isNull())
                    arguments.insert(argi + 1, QLatin1String("--") + name);
            }
            continue;
        }

        if (!opt.multiple() && opt.count() < argRead + 1) {
            m_errorString = QObject::tr("Too many arguments for option %1").arg(opt.name());
            return false;
        }
        bool ok;
        switch (opt.type(argRead)) {
        case Bool:
            value = stringToBool(&ok, arg);
            break;
        case Int:
            value = arg.toInt(&ok);
            break;
        case LongLong:
            value = arg.toLongLong(&ok);
            break;
        case Double:
            value = arg.toDouble(&ok);
            break;
        case Char:
            value = stringToChar(&ok, arg);
            break;
        case String:
            value = arg; ok = true;
            break;
        default:
            QVariant::Type variantType = QVariant::Type(opt.type(argRead));

            if (variantType == QVariant::Invalid)
                m_errorString = QObject::tr("Unknown type");
            else
                m_errorString = QObject::tr("Unsupported type %1").
                        arg(QVariant::typeToName(variantType));

            return false;
        }
        if (!ok) {
            m_errorString = QObject::tr("Type mismatch for argument %1 (expected %2)").
                    arg(arg).
                    arg(QVariant::typeToName((QVariant::Type)opt.type(argRead)));
            return false;
        }

        if (opt.d->single) {
            if (opt.multiple()) {
                list.append(value);
                singleValue = list;
            } else {
                singleValue = value;
            }
        } else {
            if (!opt.multiple() || argRead + 1 < opt.count()) {
                multiValue[opt.name(argRead)] = value;
            } else {
                QString key = opt.name(opt.count() - 1);
                list.append(value);
                multiValue[key] = list;
            }
        }

        if (!opt.multiple() && argRead == opt.count() - 1) {
            QString name = opt.name();
            if (m_values.contains(name)) {
                m_errorString = QObject::tr("Redeclaration of option %1").arg(name);
                return false;
            }
            m_values.insert(opt.name(), opt.d->single ? singleValue : QVariant(multiValue));
            singleValue.clear();
            multiValue.clear();
            list.clear();
            opt = nameToOption.value(m_defaultOption, Option());
            argRead = -1;
        }

        argRead++;
    }

    if ( (opt.multiple() && argRead >= opt.count()-1) || (!opt.multiple() && argRead == opt.count()) ) {
        QString name = opt.name();
        if (m_values.contains(name)) {
            m_errorString = QObject::tr("Redeclaration of option %1").arg(name);
            return false;
        }
        m_values.insert(opt.name(), opt.d->single ? singleValue : QVariant(multiValue));
        singleValue.clear();
        multiValue.clear();
        list.clear();
    } else {
        m_errorString = QObject::tr("Not enough arguments for option %1").arg(opt.name());
        return false;
    }

    return true;
}

QVariantMap Options::values() const
{
    return m_values;
}

Option::Option() :
    d(new OptionData)
{
}

Option::Option(const QString &name) :
    d(new OptionData)
{
    d->name = name;
}

Option::Option(const QString &name, const QString &description) :
    d(new OptionData)
{
    d->name = name;
    d->description = description;
}

Option::Option(const QChar shortName, const QString &name, const QString &description) :
    d(new OptionData)
{
    d->shortName = shortName;
    d->name = name;
    d->description = description;
}

Option::Option(const Option &other) :
    d(other.d)
{
}

Option & Option::operator=(const Option &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

Option::~Option()
{
}

QString Option::description() const
{
    return d->description;
}

void Option::setDescription(const QString &description)
{
    d->description = description;
}

QString Option::name() const
{
    return d->name;
}

QChar Option::shortName() const
{
    return d->shortName;
}

void Option::setShortName(QChar c)
{
    d->shortName = c;
}

bool Option::multiple() const
{
    return d->multiple;
}

void Option::setMultiple(bool b)
{
    d->multiple = b;
}

void Option::addValue(Options::Type type, const QString &name)
{
    d->values.append(QPair<Options::Type, QString>(type, name));
}

int Option::count() const
{
    return d->values.count();
}

Options::Type Option::type(int index) const
{
    return (d->multiple && index >= count()) ? d->values[count() - 1].first : d->values[index].first;
}

QString Option::name(int index) const
{
    return d->values[index].second;
}

bool Option::isSingle() const
{
    return d->single;
}

void Option::setSingle(bool b)
{
    d->single = b;
}

bool Option::isValid()
{
    return !d->name.isEmpty();
}
