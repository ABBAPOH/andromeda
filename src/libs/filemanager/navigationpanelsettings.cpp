#include "navigationpanelsettings.h"

namespace FileManager {

class NavigationPanelSettingsPrivate
{
public:
    QList<NavigationModel *> models;

    NavigationModel::StandardLocations locations;
};

} // namespace FileManager

using namespace FileManager;

/*!
    \class FileManager::NavigationPanelSettings

    \brief NavigationPanelSettings is a class for changing default folders
    contained in NavigationModels.

    This class holds all NavigationModels in an application and changes default
    folders in all of them.
*/

/*!
    Returns pointer to the static NavigationPanelSettings instance.
*/
NavigationPanelSettings * NavigationPanelSettings::globalSettings()
{
    static NavigationPanelSettings *instance = 0;
    if (!instance)
        instance = new NavigationPanelSettings();
    return instance;
}

/*!
    Returns flags that specify standard locations added to a model.
*/
NavigationModel::StandardLocations NavigationPanelSettings::standardLocations() const
{
    Q_D(const NavigationPanelSettings);

    return d->locations;
}

/*!
    Set flags that specify standard locations added to a model.
*/
void NavigationPanelSettings::setStandardLocations(NavigationModel::StandardLocations locations)
{
    Q_D(NavigationPanelSettings);

    if (d->locations == locations)
        return;

    d->locations = locations;

    foreach (NavigationModel *model, d->models) {
        model->setStandardLocations(locations);
    }
}

/*!
    \internal

    Creates NavigationPanelSettings.
*/
NavigationPanelSettings::NavigationPanelSettings() :
    d_ptr(new NavigationPanelSettingsPrivate)
{
    Q_D(NavigationPanelSettings);

    d->locations = NavigationModel::StandardLocations(NavigationModel::DesktopLocation |
                                                      NavigationModel::DocumentsLocation |
                                                      NavigationModel::HomeLocation |
                                                      NavigationModel::ApplicationsLocation);
}

/*!
    \internal

    Destroys NavigationPanelSettings.
*/
NavigationPanelSettings::~NavigationPanelSettings()
{
    delete d_ptr;
}

void NavigationPanelSettings::addModel(NavigationModel *model)
{
    Q_D(NavigationPanelSettings);

    if (!d->models.contains(model))
        d->models.append(model);
}

void NavigationPanelSettings::removeModel(NavigationModel *model)
{
    Q_D(NavigationPanelSettings);

    d->models.removeOne(model);
}
