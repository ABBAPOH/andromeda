#ifndef IOPTIONSPAGE_H
#define IOPTIONSPAGE_H

#include <coreplugin_global.h>

#include <QtCore/QObject>

class QIcon;

namespace CorePlugin {

class COREPLUGIN_EXPORT IOptionsPage : public QObject
{
    Q_OBJECT
public:
    IOptionsPage(const QString &id, const QString &category, QObject *parent = 0);
    virtual ~IOptionsPage();

    QString id() const { return m_id; }
    virtual QString displayName() const = 0;
    virtual QString category() const { return m_category; }
    virtual QString displayCategory() const = 0;
    virtual QIcon categoryIcon() const = 0;

    virtual QWidget *createPage(QWidget *parent) = 0;

signals:

public slots:

private:
    QString m_id;
    QString m_category;
};

} // namespace CorePlugin

#endif // IOPTIONSPAGE_H
