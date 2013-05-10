#ifndef FILEMANAGERSETTINGS_H
#define FILEMANAGERSETTINGS_H

#include "filemanager_global.h"

#include <QtCore/QSize>

namespace FileManager {

class FileManagerSettingsPrivate;
class FILEMANAGER_EXPORT FileManagerSettings
{
    Q_DECLARE_PRIVATE(FileManagerSettings)

public:
    static FileManagerSettings *globalSettings();

    bool warnOnFileRemove() const;
    void setWarnOnFileRemove(bool);

    bool warnOnExtensionChange() const;
    void setWarnOnExtensionChange(bool);

protected:
    FileManagerSettings();

    FileManagerSettingsPrivate *d_ptr;

    friend class FileManagerWidget;
};

} // namespace FileManager

#endif // FILEMANAGERSETTINGS_H
