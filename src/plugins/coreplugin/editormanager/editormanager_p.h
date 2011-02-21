#ifndef EDITORMANAGER_P_H
#define EDITORMANAGER_P_H

#include <QtCore/QMultiHash>

#include "editormanager.h"

namespace CorePlugin
{

class EditorManagerPrivate
{
public:
    QMultiHash<QString, IEditorFactory *> factories;
};

} // namespace CorePlugin

#endif // EDITORMANAGER_P_H
