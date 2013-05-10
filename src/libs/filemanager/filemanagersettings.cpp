#include "filemanagersettings.h"
#include "filemanagersettings_p.h"

#include "filemanagerwidget.h"
#include "filemanagerwidget_p.h"

using namespace FileManager;

FileManagerSettings::FileManagerSettings() :
    d_ptr(new FileManagerSettingsPrivate)
{
    Q_D(FileManagerSettings);

    d->warnOnFileRemove = true;
    d->warnOnExtensionChange = true;
}

FileManagerSettings *FileManagerSettings::globalSettings()
{
    static FileManagerSettings *instance = 0;
    if (!instance)
        instance = new FileManagerSettings();
    return instance;
}

bool FileManagerSettings::warnOnFileRemove() const
{
    Q_D(const FileManagerSettings);

    return d->warnOnFileRemove;
}

void FileManagerSettings::setWarnOnFileRemove(bool warn)
{
    Q_D(FileManagerSettings);

    d->warnOnFileRemove = warn;
}

bool FileManagerSettings::warnOnExtensionChange() const
{
    Q_D(const FileManagerSettings);

    return d->warnOnExtensionChange;
}

void FileManagerSettings::setWarnOnExtensionChange(bool warn)
{
    Q_D(FileManagerSettings);

    d->warnOnExtensionChange = warn;
}
