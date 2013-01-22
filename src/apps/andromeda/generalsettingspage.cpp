#include "generalsettingspage.h"

#include "generalsettingswidget.h"

using namespace Andromeda;

GeneralSettingsPage::GeneralSettingsPage(QObject *parent) :
    SettingsPage(QLatin1String("1.General"), QLatin1String(" General"), parent)
{
}

QString Andromeda::GeneralSettingsPage::name() const
{
    return tr("General");
}

QIcon GeneralSettingsPage::icon() const
{
    return QIcon(":/andromeda/icons/andromeda.png");
}

QString GeneralSettingsPage::categoryName() const
{
    return tr("General");
}

QIcon GeneralSettingsPage::categoryIcon() const
{
    return QIcon(":/andromeda/icons/andromeda.png");
}

QWidget * GeneralSettingsPage::createPage(QWidget *parent)
{
    return new GeneralSettingsWidget(parent);
}
