#ifndef CORE_H
#define CORE_H

#include "coreplugin_global.h"

#include "editormanager/editormanager.h"

#include <registrationmanager.h>

namespace CorePlugin
{

class CorePrivate;
class COREPLUGIN_EXPORT Core : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Core);
    Q_DISABLE_COPY(Core);

public:
    explicit Core(QObject *parent = 0);
    ~Core();

    static Core *instance();

    virtual EditorManager *editorManager();
    virtual RegistrationSystem::RegistrationManager *registrationManager();

signals:

public slots:

protected:
    CorePrivate *d_ptr;
};

} // namespace CorePlugin

#endif // CORE_H
