#include "isettingspage.h"

#include <pluginmanager.h>

using namespace CorePlugin;

ISettingsPage::ISettingsPage(const QString &id, const QString &category, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_category(category)
{
}

ISettingsPage::~ISettingsPage()
{
}

