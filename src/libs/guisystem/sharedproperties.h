#ifndef SHAREDPROPERTIES_H
#define SHAREDPROPERTIES_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace GuiSystem {

class SharedPropertiesPrivate;
class SharedProperties : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SharedProperties)
    Q_DISABLE_COPY(SharedProperties)
public:
    explicit SharedProperties(QObject *parent = 0);
    ~SharedProperties();

    bool addObject(const QString &key, QObject *object);
    bool addObject(const QString &key, QObject *object, const QByteArray &propertyName);
    void removeObject(QObject *object);
    void removeAll();

    QVariant value(const QString &key, const QVariant &defaulValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);
    void clear();

    QString group() const;
    void beginGroup(const QString &group);
    void endGroup();

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
