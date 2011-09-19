#include "filemanagersettingspage.h"

#include <QtGui/QFileIconProvider>
#include <QtGui/QLabel>

using namespace FileManagerPlugin;

FileManagerSettingsPage::FileManagerSettingsPage(QObject *parent) :
    ISettingsPage("File Manager", "FileManager", parent)
{
}

QString FileManagerSettingsPage::name() const
{
    return tr("File Manager");
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
    QLabel *label = new QLabel("Not implemented", parent);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

QIcon FileManagerSettingsPage::categoryIcon() const
{
    return icon();
}
