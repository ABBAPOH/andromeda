#include "editormanager.h"

#include "abstracteditorfactory.h"

#include <QtCore/QStringList>

namespace CorePlugin {

class EditorManagerPrivate
{
public:
    QHash<QString, AbstractEditorFactory *> factories;
    QHash<QString, AbstractEditorFactory *> factoriesById;
};

} // namespace CorePlugin

using namespace CorePlugin;

EditorManager::EditorManager(QObject *parent) :
    QObject(parent),
    d_ptr(new EditorManagerPrivate)
{
}

EditorManager::~EditorManager()
{
    delete d_ptr;
}

QList<AbstractEditorFactory *> EditorManager::factories() const
{
    return d_func()->factories.values();
}

AbstractEditorFactory * EditorManager::factory(const QString &mimeType) const
{
    return d_func()->factories.value(mimeType);
}

AbstractEditorFactory * EditorManager::factoryById(const QString &id) const
{
    return d_func()->factoriesById.value(id);
}

void EditorManager::addFactory(AbstractEditorFactory *factory)
{
    Q_D(EditorManager);

    if (!factory)
        return;

    foreach (const QString &mimeType, factory->mimeTypes()) {
        d->factories.insert(mimeType, factory);
    }
    d->factoriesById.insert(factory->id(), factory);

    connect(factory, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyed(QObject*)));
}

void EditorManager::removeFactory(AbstractEditorFactory *factory)
{
    Q_D(EditorManager);

    if (!factory)
        return;

    foreach (const QString &mimeType, d->factories.keys(factory)) {
        d->factories.remove(mimeType);
    }
    d->factoriesById.remove(d->factoriesById.key(factory));

    disconnect(factory, 0, this, 0);
}

AbstractEditor * EditorManager::editor(const QString &mimeType, QWidget *parent)
{
    AbstractEditorFactory *f = factory(mimeType);
    if (f)
        return f->editor(parent);

    return 0;
}

AbstractEditor * EditorManager::editorById(const QString &id, QWidget *parent)
{
    AbstractEditorFactory *f = factoryById(id);
    if (f)
        return f->editor(parent);

    return 0;
}

void EditorManager::onDestroyed(QObject *o)
{
    removeFactory(static_cast<AbstractEditorFactory *>(o));
}
