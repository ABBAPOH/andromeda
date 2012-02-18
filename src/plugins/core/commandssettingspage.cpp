#include "commandssettingspage.h"

#include <guisystem/commandssettingswidget.h>

using namespace GuiSystem;

CommandsSettingsPage::CommandsSettingsPage(QObject *parent) :
    SettingsPage(QLatin1String("CommandSettings"), QLatin1String(" General"), parent)
{
}

QString CommandsSettingsPage::name() const
{
    return tr("Shortcut settings");
}

QIcon CommandsSettingsPage::icon() const
{
    return QIcon();
}

QString CommandsSettingsPage::categoryName() const
{
    return tr("General");
}

QIcon CommandsSettingsPage::categoryIcon() const
{
    return QIcon(":/images/icons/andromeda.png");
}

QWidget * CommandsSettingsPage::createPage(QWidget *parent)
{
    return new CommandsSettingsWidget(parent);
}
