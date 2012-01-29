#include "navigationpanelsettings.h"

namespace FileManagerPlugin {

class NavigationPanelSettingsPrivate
{
public:
    QList<NavigationModel *> models;

    NavigationModel::StandardLocations locations;
};

} // namespace FileManagerPlugin

using namespace FileManagerPlugin;

NavigationPanelSettings * NavigationPanelSettings::globalSettings()
{
    static NavigationPanelSettings *instance = 0;
    if (!instance)
        instance = new NavigationPanelSettings();
    return instance;
}

NavigationPanelSettings::NavigationPanelSettings() :
    d_ptr(new NavigationPanelSettingsPrivate)
{
    Q_D(NavigationPanelSettings);

    d->locations = NavigationModel::StandardLocations(NavigationModel::DesktopLocation |
                                                      NavigationModel::DocumentsLocation |
                                                      NavigationModel::HomeLocation |
                                                      NavigationModel::ApplicationsLocation);
}

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

NavigationModel::StandardLocations NavigationPanelSettings::standardLocations() const
{
    Q_D(const NavigationPanelSettings);

    return d->locations;
}

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
