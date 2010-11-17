#include "editormanager.h"
#include "editormanager_p.h"

#include "ieditorfactory.h"

#include <QtCore/QDebug>
#include <icore.h>

using namespace Core;

// ============= EditorManager =============

EditorManager::EditorManager(QObject *parent) :
    QObject(parent),
    d_ptr(new EditorManagerPrivate)
{
    ICore *core = ICore::instance();
    core->registrationManager()->addRegistrator(d_ptr, "EditorFactory");
}

EditorManager::~EditorManager()
{
    delete d_ptr;
}

IEditor *EditorManager::openEditor(const QString &file)
{
    Q_D(EditorManager);
}

// ============= EditorManagerPrivate =============

bool EditorManagerPrivate::registerObject(QObject *object)
{
    IEditorFactory *factory = qobject_cast<IEditorFactory *>(object);
    if (!factory) {
        qWarning() << "Attempt to register object not of class IEditorFactory: " <<
                object->metaObject()->className();
        return false;
    }

    factories.insert(factory->type(), factory);

    return true;
}

bool EditorManagerPrivate::unregisterObject(QObject *object)
{
    IEditorFactory *factory = qobject_cast<IEditorFactory *>(object);
    if (factory)
        factories.remove(factory->type(), factory);
}
