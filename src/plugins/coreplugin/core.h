#ifndef CORE_H
#define CORE_H

#include "coreplugin_global.h"

#include <ExtensionSystem>

#include "editormanager/editormanager.h"

namespace CorePlugin {

class PerspectiveManager;
class CorePrivate;
class COREPLUGIN_EXPORT Core : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Core);
    Q_DISABLE_COPY(Core);

    Q_PROPERTY(EditorManager *editorManager READ editorManager);

public:
    explicit Core(QObject *parent = 0);
    ~Core();

    static Core *instance()
    {
        return ExtensionSystem::PluginManager::instance()->object<Core>("core");
    }

    EditorManager *editorManager();
    PerspectiveManager *perspectiveManager() const;

    QObject *getObject(const QString &name);

signals:

public slots:

protected:
    CorePrivate *d_ptr;
};

} // namespace CorePlugin

#endif // CORE_H
