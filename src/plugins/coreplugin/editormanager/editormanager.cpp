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

bool factoriesLessThan(IEditorFactory *f1, IEditorFactory *f2)
{
    return f1->priority() < f2->priority();
}

// we also need to return group of this editors
QList<IEditor *> EditorManager::openEditors(const QUrl &url)
{
    Q_D(EditorManager);
    QList<IEditorFactory *> factories = d->factories.values();
    QList<IEditor *> editors;
    qSort(factories.begin(), factories.end(), factoriesLessThan);
    foreach (IEditorFactory *factory, factories) {
        if (factory->canOpen(url)) {
            IEditor *editor = factory->open(url);
            editors.append(editor);
        }
    }
    return editors;
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
