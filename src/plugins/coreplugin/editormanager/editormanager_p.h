#ifndef EDITORMANAGER_P_H
#define EDITORMANAGER_P_H

#include <registrationmanager.h>
#include <QtCore/QMultiHash>

#include "editormanager.h"

namespace CorePlugin
{

class EditorManagerPrivate: public RegistrationSystem::IRegistrator
{
public:
    virtual bool registerObject(QObject *object);
    virtual bool unregisterObject(QObject *object);

    QMultiHash<QString, IEditorFactory *> factories;
};

} // namespace CorePlugin

#endif // EDITORMANAGER_P_H
