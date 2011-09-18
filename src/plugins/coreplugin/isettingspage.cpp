#include "isettingspage.h"

#include <pluginmanager.h>

#include "settingsdialog_p.h"

using namespace CorePlugin;

ISettingsPage::ISettingsPage(const QString &id, const QString &category, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_category(category)
{
    ExtensionSystem::PluginManager::instance()->object<CategoryModel>("CategoryModel")->addPage(this);
}

ISettingsPage::~ISettingsPage()
{
    ExtensionSystem::PluginManager::instance()->object<CategoryModel>("CategoryModel")->removePage(this);
}

