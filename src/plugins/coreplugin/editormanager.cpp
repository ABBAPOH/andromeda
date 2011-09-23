#include "editormanager.h"

#include "abstracteditorfactory.h"

#include <QtCore/QStringList>

using namespace CorePlugin;

EditorManager::EditorManager(QObject *parent) :
    QObject(parent)
{
}

QList<AbstractEditorFactory *> EditorManager::factories() const
{
    return m_factories.values();
}

AbstractEditorFactory * EditorManager::factory(const QString &mimeType) const
{
    return m_factories.value(mimeType);
}

void EditorManager::addFactory(AbstractEditorFactory *factory)
{
    if (!factory)
        return;

    foreach (const QString &mimeType, factory->mimeTypes()) {
        m_factories.insert(mimeType, factory);
    }
    connect(factory, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyed(QObject*)));
}

void EditorManager::removeFactory(AbstractEditorFactory *factory)
{
    if (!factory)
        return;

    foreach (const QString &mimeType, m_factories.keys(factory)) {
        m_factories.remove(mimeType);
    }

    disconnect(factory, 0, this, 0);
}

AbstractEditor * EditorManager::editor(const QString &mimeType, QWidget *parent)
{
    AbstractEditorFactory *f = factory(mimeType);
    if (f)
        return f->editor(parent);

    return 0;
}

void EditorManager::onDestroyed(QObject *o)
{
    removeFactory(static_cast<AbstractEditorFactory *>(o));
}
