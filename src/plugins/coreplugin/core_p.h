#ifndef CORE_P_H
#define CORE_P_H

#include "core.h"

namespace Core
{

class CorePrivate
{
public:
    EditorManager *editorManager;
    RegistrationManager *registrationManager;
};

} // namespace Core

#endif // CORE_P_H
