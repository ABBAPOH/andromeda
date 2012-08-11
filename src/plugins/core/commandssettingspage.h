#ifndef COMMANDSSETTINGSPAGE_H
#define COMMANDSSETTINGSPAGE_H

#include <guisystem/settingspage.h>

namespace Core {

class CommandsSettingsPage : public GuiSystem::SettingsPage
{
    Q_OBJECT

public:
    explicit CommandsSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

} // namespace Core

#endif // COMMANDSSETTINGSPAGE_H
