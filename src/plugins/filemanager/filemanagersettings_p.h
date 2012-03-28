#ifndef FILEMANAGERSETTINGS_P_H
#define FILEMANAGERSETTINGS_P_H

#include "filemanagersettings.h"

#include <QtCore/QList>

namespace FileManager {

class FileManagerWidget;
class FileManagerSettingsPrivate
{
public:
    void addWidget(FileManagerWidget *widget);
    void removeWidget(FileManagerWidget *widget);

public:
    QList<FileManagerWidget *> widgets;

    QSize gridSize;
    FileManagerSettings::Flow flow;
    QSize iconSizes[FileManagerSettings::ViewCount];
    bool itemsExpandable;
    bool warnOnFileRemove;
    bool warnOnExtensionChange;
};

} // namespace FileManager

#endif // FILEMANAGERSETTINGS_P_H
