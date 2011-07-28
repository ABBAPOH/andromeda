#ifndef MUTABLEPLUGINSPEC_H
#define MUTABLEPLUGINSPEC_H

#include "pluginspec.h"

namespace ExtensionSystem {

class EXTENSIONSYSTEM_EXPORT MutablePluginSpec : public PluginSpec
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PluginSpec)
    Q_DISABLE_COPY(MutablePluginSpec)

public:
    explicit MutablePluginSpec(QObject *parent = 0);

    void setName(const QString &name);
    void setVersion(const Version &version);
    void setCompatibilityVersion(const Version &version);
    void setVendor(const QString &vendor);
    void setCategory(const QString &category);
    void setCopyright(const QString &copyright);
    void setLicense(const QString &license);
    void setDescription(const QString &description);
    void setUrl(const QString &url);

    void setDependencies(const QList<PluginDependency> &dependencies);
};

} // namespace ExtensionSystem

#endif // MUTABLEPLUGINSPEC_H
