#ifndef CORE_H
#define CORE_H

#include "coreplugin_global.h"

#include <extensionsystem/pluginmanager.h>

namespace CorePlugin {

class EditorManager;
class PerspectiveManager;
class Settings;

class CorePrivate;
class COREPLUGIN_EXPORT Core : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Core)
    Q_DISABLE_COPY(Core)

public:
    explicit Core(QObject *parent = 0);
    ~Core();

    static Core *instance()
    {
        return ExtensionSystem::PluginManager::instance()->object<Core>("core");
    }

    EditorManager *editorManager() const;

    Settings *settings() const;

    QObject *getObject(const QString &name);

protected:
    CorePrivate *d_ptr;
};

} // namespace CorePlugin

#endif // CORE_H
