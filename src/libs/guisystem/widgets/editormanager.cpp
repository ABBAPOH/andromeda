#include "editormanager.h"

#include <QtCore/QDebug>
#include <QtCore/QMap>

#include "abstracteditorfactory.h"

namespace GuiSystem {

class EditorManagerPrivate
{
public:
    QMap<QByteArray, AbstractEditorFactory *> factories;
};

} // namespace GuiSystem

using namespace GuiSystem;

/*!
  \class GuiSystem::EditorManager

  \brief The DocumentManager class contains all editors' factories.
*/

/*!
  \brief Creates a EditorManager with the given \a parent.
*/
EditorManager::EditorManager(QObject *parent) :
    QObject(parent),
    d_ptr(new EditorManagerPrivate)
{
}

/*!
  \brief Destroys DocumentManager.
*/
EditorManager::~EditorManager()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(EditorManager, staticInstance)

/*!
  \brief Returns static instance of an editor.
*/
EditorManager * EditorManager::instance()
{
    return staticInstance();
}

AbstractEditorFactory *EditorManager::factory(const QByteArray &id) const
{
    Q_D(const EditorManager);
    return d->factories.value(id);
}

AbstractEditor *EditorManager::createEditor(const QByteArray &id, QWidget *parent) const
{
    AbstractEditorFactory *factory = this->factory(id);
    if (!factory)
        return 0;

    return factory->editor(parent);
}

void EditorManager::addFactory(AbstractEditorFactory *factory)
{
    Q_D(EditorManager);
    if (!factory)
        return;

    QByteArray id = factory->id();
    if (d->factories.contains(id))
        qWarning() << "EditorManager::addFactory" << "factory with id" << id << "already added.";

    d->factories.insert(id, factory);
    connect(factory, SIGNAL(destroyed(QObject*)), SLOT(onFactoryDestroyed(QObject*)));
}

void EditorManager::removeFactory(AbstractEditorFactory *factory)
{
    Q_D(EditorManager);
    if (!factory)
        return;

    QByteArray id = d->factories.key(factory);
    d->factories.remove(id);
}

void EditorManager::onFactoryDestroyed(QObject *object)
{
    removeFactory(static_cast<AbstractEditorFactory*>(object));
}

