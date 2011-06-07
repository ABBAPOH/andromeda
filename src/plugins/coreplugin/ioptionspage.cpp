#include "ioptionspage.h"

#include <pluginmanager.h>

#include "settingsdialog_p.h"

using namespace CorePlugin;

IOptionsPage::IOptionsPage(const QString &id, const QString &category, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_category(category)
{
    ExtensionSystem::PluginManager::instance()->object<CategoryModel>("CategoryModel")->addPage(this);
}

IOptionsPage::~IOptionsPage()
{
    ExtensionSystem::PluginManager::instance()->object<CategoryModel>("CategoryModel")->removePage(this);
}

