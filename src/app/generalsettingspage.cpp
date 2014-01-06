#include "generalsettingspage.h"

#include "generalsettingswidget.h"

using namespace Andromeda;

/*!
    \class Andromeda::GeneralSettingsPage
    Implements Parts::SettingsPage to create GeneralSettingsWidget.
*/

/*!
    Creates GeneralSettingsPage with the given \a parent.
*/
GeneralSettingsPage::GeneralSettingsPage(QObject *parent) :
    SettingsPage(QLatin1String("1.General"), QLatin1String(" General"), parent)
{
}

/*!
    \reimp
*/
QString Andromeda::GeneralSettingsPage::name() const
{
    return tr("General");
}

/*!
    \reimp
*/
QIcon GeneralSettingsPage::icon() const
{
    return QIcon(":/andromeda/icons/andromeda.png");
}

/*!
    \reimp
*/
QString GeneralSettingsPage::categoryName() const
{
    return tr("General");
}

/*!
    \reimp
*/
QIcon GeneralSettingsPage::categoryIcon() const
{
    return QIcon(":/andromeda/icons/andromeda.png");
}

/*!
    \reimp
*/
QWidget * GeneralSettingsPage::createPage(QWidget *parent)
{
    return new GeneralSettingsWidget(parent);
}
