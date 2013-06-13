#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include <Parts/SettingsPage>

namespace Andromeda {

class GeneralSettingsPage : public Parts::SettingsPage
{
    Q_OBJECT
public:
    explicit GeneralSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

} // namespace Andromeda

#endif // GENERALSETTINGSPAGE_H
