#include "isettingspage.h"

#include <extensionsystem/pluginmanager.h>

using namespace CorePlugin;

/*!
    \class ISettingsPage

    \brief ISettingsPage represents single page in SettingsDialog
*/

/*!
    \brief Creates a ISettingsPage with the given \a id and \a category.

    Pages are separated by categories in the SettingsDialog. The first page added to a category represents
    icon and name of the category.
*/
ISettingsPage::ISettingsPage(const QString &id, const QString &category, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_category(category)
{
}

/*!
    \fn QString ISettingsPage::id() const

    \brief Returns the id of this page. It should be unique for each page.
*/

/*!
    \fn QString ISettingsPage::category() const

    \brief Returns the category of this page.
*/

/*!
    \fn QString ISettingsPage::name() const

    \brief Returns name of a page that is displayed on tab in SettingsDialog.
*/

/*!
    \fn QString ISettingsPage::icon() const

    \brief Returns icon of a page that is displayed as a window icon when page is active.
*/

/*!
    \fn QString ISettingsPage::categoryName() const

    \brief Returns name of a category of this page.

    This function only called for 'main' pages - pages that were added first to category.
*/

/*!
    \fn QString ISettingsPage::categoryIcon() const

    \brief Returns icon of a category of this page.

    This function only called for 'main' pages - pages that were added first to category.
*/

/*!
    \fn QString ISettingsPage::createPage(QWidget *parent) const

    \brief Reimplement to create new widget within tab.

    This function is called each time new SettingsDialog created.
*/
