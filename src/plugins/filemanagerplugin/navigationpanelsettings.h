#ifndef NAVIGATIONPANELSETTINGS_H
#define NAVIGATIONPANELSETTINGS_H

#include "navigationmodel.h"

namespace FileManagerPlugin {

class NavigationPanelSettingsPrivate;
class NavigationPanelSettings
{
    Q_DECLARE_PRIVATE(NavigationPanelSettings)
    Q_DISABLE_COPY(NavigationPanelSettings)

public:
    static NavigationPanelSettings *globalSettings();

    void addModel(NavigationModel *model);
    void removeModel(NavigationModel *model);

    NavigationModel::StandardLocations standardLocations() const;
    void setStandardLocations(NavigationModel::StandardLocations locations);

protected:
    NavigationPanelSettings();
    ~NavigationPanelSettings();

protected:
    NavigationPanelSettingsPrivate *d_ptr;

    friend class NavigationModel;
};

} //namespace FileManagerPlugin

#endif // NAVIGATIONPANELSETTINGS_H
