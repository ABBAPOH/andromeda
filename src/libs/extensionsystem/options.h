#ifndef QOPTIONS_H
#define QOPTIONS_H

#include "extensionsystem_global.h"

#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariant>

namespace ExtensionSystem {

class Option;
class EXTENSIONSYSTEM_EXPORT Options
{
    Q_DISABLE_COPY(Options)

public:
    enum Type {
        Bool = QVariant::Bool,
        Int = QVariant::Int,
        LongLong = QVariant::LongLong,
        Double = QVariant::Double,
        Char = QVariant::Char,
        String = QVariant::String
    };

    Options();

    bool addOption(const QChar shortName, const QString &name, Type type, const QString &description);
    bool addOption(const QString &name, Type type, const QString &description);
    bool addOption(const Option &option);

    void clear();

    QString defaultOption() const;
    void setDefaultOption(const QString &name);

    bool parse(const QStringList &arguments);

    QVariantMap values() const;

private:
    QMap<QString, Option> nameToOption;
    QMap<QChar, QString> shortToName;

    QString m_defaultOption;

    QVariantMap m_values;
};

class OptionData;
class EXTENSIONSYSTEM_EXPORT Option
{
public:
    explicit Option(const QString &name);
    Option(const QString &name, const QString &description);
    Option(const QChar shortName, const QString &name, const QString &description);
    Option(const Option &other);
    Option &operator=(const Option &other);
    ~Option();

    QString description() const;
    void setDescription(const QString &description);

    QString name() const;

    QChar shortName() const;
    void setShortName(QChar c);

    bool multiple() const;
    void setMultiple(bool);

    void addValue(Options::Type type, const QString &name);
    int count() const;
    Options::Type type(int index);
    QString name(int index);

    bool isSingle() const;
    void setSingle(bool b);

private:
    bool isValid();

private:
    QSharedDataPointer<OptionData> d;

    Option();

    friend class Options;
};

} // namespace ExtensionSystem

#endif // QOPTIONS_H
