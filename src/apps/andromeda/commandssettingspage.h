#ifndef COMMANDSSETTINGSPAGE_H
#define COMMANDSSETTINGSPAGE_H

#include <guisystem/settingspage.h>

namespace Andromeda {

class CommandsSettingsPage : public GuiSystem::SettingsPage
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
