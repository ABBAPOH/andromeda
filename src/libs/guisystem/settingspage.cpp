#include "settingspage.h"

using namespace GuiSystem;

/*!
    \class GuiSystem::SettingsPage

    \brief SettingsPage represents single page in SettingsWindow
*/

/*!
    \brief Creates a SettingsPage with the given \a id and \a category.

    Pages are separated by categories in the SettingsWindow. The first page added to a category represents
    icon and name of the category.
*/
SettingsPage::SettingsPage(const QString &id, const QString &category, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_category(category)
{
}

/*!
    \fn QString SettingsPage::id() const

    \brief Returns the id of this page. It should be unique for each page.
*/

/*!
    \fn QString SettingsPage::category() const

    \brief Returns the category of this page.
*/

/*!
    \fn QString SettingsPage::name() const

    \brief Returns name of a page that is displayed on tab in SettingsWindow.
*/

/*!
    \fn QString SettingsPage::icon() const

    \brief Returns icon of a page that is displayed as a window icon when page is active.
*/

/*!
    \fn QString SettingsPage::categoryName() const

    \brief Returns name of a category of this page.

    This function only called for 'main' pages - pages that were added first to category.
*/

/*!
    \fn QString SettingsPage::categoryIcon() const

    \brief Returns icon of a category of this page.

    This function only called for 'main' pages - pages that were added first to category.
*/

/*!
    \fn QString SettingsPage::createPage(QWidget *parent) const

    \brief Reimplement to create new widget within tab.

    This function is called each time new SettingsWindow created.
*/
