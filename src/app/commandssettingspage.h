#ifndef COMMANDSSETTINGSPAGE_H
#define COMMANDSSETTINGSPAGE_H

#include <Parts/SettingsPage>

namespace Andromeda {

class CommandsSettingsPage : public Parts::SettingsPage
{
    Q_OBJECT
    Q_DISABLE_COPY(CommandsSettingsPage)
public:
    explicit CommandsSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

} // namespace Andromeda

#endif // COMMANDSSETTINGSPAGE_H
