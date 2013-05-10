#ifndef SHAREDPROPERTIES_H
#define SHAREDPROPERTIES_H

#include "../guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>

class QSettings;

namespace GuiSystem {

class SharedPropertiesPrivate;
class GUISYSTEM_EXPORT SharedProperties : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SharedProperties)
    Q_DISABLE_COPY(SharedProperties)
public:
    explicit SharedProperties(QObject *parent = 0);
    ~SharedProperties();

    bool addProperty(const QString &key, QObject *object);
    bool addProperty(const QString &key, QObject *object, const QByteArray &propertyName);
    void removeProperty(const QString &key, QObject *object);
    void removeProperty(const QString &key, QObject *object, const QByteArray &propertyName);
    void removeProperties(QObject *object);
    void removeAll();

    QVariant value(const QString &key, const QVariant &defaulValue = QVariant()) const;
    QVariantMap values() const;

    QString group() const;
    void beginGroup(const QString &group);
    void endGroup();

    void read(const QSettings *settings);
    void write(QSettings *settings);

public slots:
    void setValue(const QString &key, const QVariant &value);
    void updateValue(const QString &key, const QVariant &value);
    void clear();

signals:
    void valueChanged(const QString &key, const QVariant &value);

protected slots:
    void onDestroyed(QObject *object);
    void onValueChanged();

private:
    SharedPropertiesPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // SHAREDPROPERTIES_H
