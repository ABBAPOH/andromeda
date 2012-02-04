#include "filemanagersettingspage.h"

#include "filemanagersettingswidget.h"

#include <QtGui/QFileIconProvider>
#include <QtGui/QLabel>

using namespace FileManagerPlugin;

FileManagerSettingsPage::FileManagerSettingsPage(QObject *parent) :
    SettingsPage("File Manager", "FileManager", parent)
{
}

QString FileManagerSettingsPage::name() const
{
    return tr("Global");
}

QString FileManagerSettingsPage::categoryName() const
{
    return tr("File Manager");
}

QIcon FileManagerSettingsPage::icon() const
{
    return QFileIconProvider().icon(QFileIconProvider::Folder);
}

QWidget *FileManagerSettingsPage::createPage(QWidget *parent)
{
    return new FileManagerSettingsWidget(parent);
}

QIcon FileManagerSettingsPage::categoryIcon() const
{
    return icon();
}
