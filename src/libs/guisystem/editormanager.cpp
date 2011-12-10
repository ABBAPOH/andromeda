#include "editormanager.h"

#include "abstracteditorfactory.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

namespace GuiSystem {

class EditorManagerPrivate
{
public:
    QHash<QString, AbstractEditorFactory *> factories;
    QHash<QString, AbstractEditorFactory *> factoriesById;
    QHash<QString, AbstractViewFactory *> viewFactories;
};

} // namespace CorePlugin

using namespace GuiSystem;

/*!
  \class EditorManager

  \brief The EditorManager class contains all editors' and views' factories.
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
  \brief Destroys EditorManager.
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

/*!
  \brief Creates new editor with given \a id and \a parent.
*/
AbstractEditor * EditorManager::editorForId(const QString &id, QWidget *parent)
{
    AbstractEditorFactory *f = factoryById(id);
    if (f)
        return f->editor(parent);

    return 0;
}

/*!
  \brief Creates new editor with \a parent that can handle given \a mimeType.
*/
AbstractEditor * EditorManager::editorForMimeType(const QString &mimeType, QWidget *parent)
{
    AbstractEditorFactory *f = factoryForMimeType(mimeType);
    if (f)
        return f->editor(parent);

    return 0;
}

/*!
  \brief Creates new editor with \a parent that can handle given \a url.
*/
AbstractEditor * EditorManager::editorForUrl(const QUrl &url, QWidget *parent)
{
    AbstractEditorFactory *f = factoryForUrl(url);
    if (f)
        return f->editor(parent);

    return 0;
}

/*!
  \brief Returns list of all factories.
*/
QList<AbstractEditorFactory *> EditorManager::factories() const
{
    return d_func()->factories.values();
}

/*!
  \brief Returns list of factories that can handle given \a mimeType.
*/
QList<AbstractEditorFactory *> EditorManager::factoriesForMimeType(const QString &mimeType) const
{
    return d_func()->factories.values(mimeType);
}

/*!
  \brief Returns factory with given \a id.
*/
AbstractEditorFactory * EditorManager::factoryById(const QString &id) const
{
    return d_func()->factoriesById.value(id);
}

/*!
  \brief Returns factory that can hanlde given \a mimeType.
*/
AbstractEditorFactory * EditorManager::factoryForMimeType(const QString &mimeType) const
{
    return d_func()->factories.value(mimeType);
}

/*!
  \brief Returns factory that can hanlde given \a url.
*/
AbstractEditorFactory * EditorManager::factoryForUrl(const QUrl &url) const
{
    if (url.scheme() == qApp->applicationName()) {
        return factoryById(url.host());
    } else {
        QString mimeType = getMimeType(url);
        return factoryForMimeType(mimeType);
    }
    return 0;
}

/*!
  \brief Adds \a factory to EditorManager.

  Factories automatically removed when being destroyed.
*/
void EditorManager::addFactory(AbstractEditorFactory *factory)
{
    Q_D(EditorManager);

    if (!factory)
        return;

    foreach (const QString &mimeType, factory->mimeTypes()) {
        d->factories.insert(mimeType, factory);
    }
    d->factoriesById.insert(factory->id(), factory);

    connect(factory, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyed1(QObject*)));
}

/*!
  \brief Removes \a factory from EditorManager.
*/
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

AbstractViewFactory * EditorManager::viewfactory(const QString &id) const
{
    return d_func()->viewFactories.value(id);
}

/*!
  \brief Adds \a factory to EditorManager.

  If you need to add EditorFactory, use EditorManager::addFactory instead.

  Factories automatically removed when being destroyed.
*/
void EditorManager::addViewFactory(AbstractViewFactory *factory)
{
    Q_D(EditorManager);

    if (!factory)
        return;

    d->viewFactories.insert(factory->id(), factory);

    connect(factory, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyed2(QObject*)));
}

/*!
  \brief Removes \a factory from EditorManager.

  To remove EditorFactory, use EditorManager::removeFactory instead.
*/
void EditorManager::removeViewFactory(AbstractViewFactory *factory)
{
    Q_D(EditorManager);

    if (!factory)
        return;

    d->viewFactories.remove(d->viewFactories.key(factory));

    disconnect(factory, 0, this, 0);
}

/*!
  \internal
*/
void EditorManager::onDestroyed1(QObject *o)
{
    removeFactory(static_cast<AbstractEditorFactory *>(o));
}

/*!
  \internal
*/
void EditorManager::onDestroyed2(QObject *o)
{
    removeViewFactory(static_cast<AbstractViewFactory *>(o));
}

/*!
  \internal
*/
QString EditorManager::getMimeType(const QUrl &url) const
{
    if (url.scheme() == QLatin1String("file")) {
        QFileInfo info(url.toLocalFile());
        if (info.isDir() && !info.isBundle())
            return QLatin1String("inode/directory");
    } else if(url.scheme() == QLatin1String("http") ||
              url.scheme() == QLatin1String("https")) {
        return QLatin1String("text/html");
    }
    return QString();
}

