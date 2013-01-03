#ifndef NAVIGATIONPANELSETTINGS_H
#define NAVIGATIONPANELSETTINGS_H

#include "filemanager_global.h"

#include "navigationmodel.h"

namespace FileManager {

class NavigationPanelSettingsPrivate;
class FILEMANAGER_EXPORT NavigationPanelSettings
{
    Q_DECLARE_PRIVATE(NavigationPanelSettings)
    Q_DISABLE_COPY(NavigationPanelSettings)

public:
    static NavigationPanelSettings *globalSettings();

    NavigationModel::StandardLocations standardLocations() const;
    void setStandardLocations(NavigationModel::StandardLocations locations);

protected:
    NavigationPanelSettings();
    ~NavigationPanelSettings();

    void addModel(NavigationModel *model);
    void removeModel(NavigationModel *model);

protected:
    NavigationPanelSettingsPrivate *d_ptr;

    friend class NavigationModel;
};

} //namespace FileManager

#endif // NAVIGATIONPANELSETTINGS_H
