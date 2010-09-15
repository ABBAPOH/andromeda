#ifndef EDITORMANAGER_P_H
#define EDITORMANAGER_P_H

#include <registrationmanager.h>
#include <QtCore/QMultiHash>

#include "editormanager.h"

namespace Core
{

class EditorManagerPrivate: public RegistrationSystem::IRegistrator
{
public:
    virtual bool registerObject(QObject *object);
    virtual bool unregisterObject(QObject *object);

    QMultiHash<QString, IEditorFactory *> factories;
};

} // namespace Core

#endif // EDITORMANAGER_P_H
