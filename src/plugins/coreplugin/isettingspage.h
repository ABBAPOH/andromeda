#ifndef IOPTIONSPAGE_H
#define IOPTIONSPAGE_H

#include <coreplugin_global.h>

#include <QtCore/QObject>

class QIcon;

namespace CorePlugin {

class COREPLUGIN_EXPORT ISettingsPage : public QObject
{
    Q_OBJECT
public:
    ISettingsPage(const QString &id, const QString &category, QObject *parent = 0);
    virtual ~ISettingsPage();

    QString id() const { return m_id; }
    QString category() const { return m_category; }

    virtual QString displayName() const = 0;
    virtual QString displayCategory() const = 0;
    virtual QIcon categoryIcon() const = 0;

    virtual QWidget *createPage(QWidget *parent) = 0;

private:
    QString m_id;
    QString m_category;
};

} // namespace CorePlugin

#endif // IOPTIONSPAGE_H
