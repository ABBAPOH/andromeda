#ifndef FILEMANAGERSETTINGS_P_H
#define FILEMANAGERSETTINGS_P_H

#include "filemanagersettings.h"

#include <QtCore/QList>

namespace FileManager {

class FileManagerWidget;
class FileManagerSettingsPrivate
{
public:
    bool warnOnFileRemove;
    bool warnOnExtensionChange;
};

} // namespace FileManager

#endif // FILEMANAGERSETTINGS_P_H
