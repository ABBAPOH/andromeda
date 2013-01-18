#include "documentmanager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include <IO/QMimeDatabase>

#include "abstractdocumentfactory.h"

namespace GuiSystem {

class DocumentManagerPrivate
{
public:
    QHash<QString, AbstractDocumentFactory *> factoriesForId;
    QHash<QString, DocumentManager::FactoryList> factoriesForMimeType;
    QHash<QString, DocumentManager::FactoryList> factoriesForScheme;
};

} // namespace GuiSystem

using namespace GuiSystem;

static inline bool editorFactoryLessThan(AbstractDocumentFactory *first, AbstractDocumentFactory *second)
{
    if (first->weight() == second->weight())
        return first < second;
    return first->weight() > second->weight();
}

/*!
  \class GuiSystem::DocumentManager

  \brief The DocumentManager class contains all documents' factories.
*/

/*!
  \brief Creates a DocumentManager with the given \a parent.
*/
DocumentManager::DocumentManager(QObject *parent) :
    QObject(parent),
    d_ptr(new DocumentManagerPrivate)
{
}

/*!
  \brief Destroys DocumentManager.
*/
DocumentManager::~DocumentManager()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(DocumentManager, staticInstance)

/*!
  \brief Returns static instance of an editor.
*/
DocumentManager * DocumentManager::instance()
{
    return staticInstance();
}

/*!
  \brief Returns factory with given \a id.
*/
AbstractDocumentFactory * DocumentManager::factoryForId(const QString &id) const
{
    return d_func()->factoriesForId.value(id);
}

/*!
  \brief Returns factory that can handle given \a mimeType.
*/
AbstractDocumentFactory * DocumentManager::factoryForMimeType(const QString &mimeType) const
{
    const FactoryList &factories = factoriesForMimeType(mimeType);
    if (factories.isEmpty())
        return 0;

    return factories.first();
}

/*!
  \brief Returns factory that can handle given \a scheme.
*/
AbstractDocumentFactory * DocumentManager::factoryForScheme(const QString &scheme) const
{
    const FactoryList &factories = factoriesForScheme(scheme);
    if (factories.isEmpty())
        return 0;

    return factories.first();
}

/*!
  \brief Returns factory that can handle given \a url.
*/
AbstractDocumentFactory * DocumentManager::factoryForUrl(const QUrl &url) const
{
    const FactoryList &factories = this->factoriesForUrl(url);
    if (factories.isEmpty())
        return 0;

    return factories.first();
}

/*!
  \brief Returns list of all factories.
*/
DocumentManager::FactoryList DocumentManager::factories() const
{
    return d_func()->factoriesForId.values();
}

/*!
  \brief Returns list of factories that can handle given \a mimeType.
*/
DocumentManager::FactoryList DocumentManager::factoriesForMimeType(const QString &mimeType) const
{
    FactoryList result;

    QMimeDatabase db;
    QMimeType mt = db.mimeTypeForName(mimeType);
    QStringList mimeTypes;
    mimeTypes.append(mimeType);
    mimeTypes.append(mt.parentMimeTypes());
    foreach (const QString &mimeType, mimeTypes) {
        const FactoryList &factories = d_func()->factoriesForMimeType.value(mimeType);
        foreach (AbstractDocumentFactory *f, factories) {
            if (!result.contains(f))
                result.append(f);
        }
    }

    qStableSort(result.begin(), result.end(), editorFactoryLessThan);

    return result;
}

/*!
  \brief Returns list of factories that can handle given \a scheme.
*/
DocumentManager::FactoryList DocumentManager::factoriesForScheme(const QString &scheme) const
{
    return d_func()->factoriesForScheme.value(scheme);
}

/*!
  \brief Returns list of factories that can handle given \a url.
*/
DocumentManager::FactoryList DocumentManager::factoriesForUrl(const QUrl &url) const
{
    QList<AbstractDocumentFactory *> result;

    if (url.scheme() == qApp->applicationName()) {
        result.append(factoryForId(url.host()));
    } else {
        QMimeDatabase db;
        QList<QMimeType> mimeTypes;

        mimeTypes.append(db.mimeTypesForFileName(QFileInfo(url.path()).fileName()));
        mimeTypes.append(db.mimeTypeForUrl(url));
        foreach (const QMimeType &mimeType, mimeTypes) {
            QString mimeTypeName = mimeType.name();
            result.append(factoriesForMimeType(mimeTypeName));
        }

        // add scheme factory last to maximize chance to
        // get "mime type editor" with same weight instead of
        // "scheme editor"
        result.append(factoriesForScheme(url.scheme()));
    }

    qStableSort(result.begin(), result.end(), editorFactoryLessThan);
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

DocumentManager::FactoryList DocumentManager::factoriesForUrls(const QList<QUrl> &urls) const
{
    if (urls.isEmpty())
        return FactoryList();

    FactoryList factories = this->factoriesForUrl(urls.first());

    foreach (const QUrl &url, urls.mid(1)) {
        if (factories.isEmpty())
            break;

        FactoryList list = this->factoriesForUrl(url);
        foreach (AbstractDocumentFactory *f, factories) {
            if (!list.contains(f))
                factories.removeOne(f);
        }
    }

    return factories;
}

/*!
  \brief Adds \a factory to DocumentManager.

  Factories automatically removed when being destroyed.
*/
void DocumentManager::addFactory(AbstractDocumentFactory *factory)
{
    Q_D(DocumentManager);

    if (!factory)
        return;

    if (d->factoriesForId.contains(factory->id())) {
        qWarning() << "DocumentManager::addFactory :"
                   << QString("Factory with id %1 already exists").arg(QString(factory->id()));
        return;
    }

    d->factoriesForId.insert(factory->id(), factory);

    foreach (const QString &mimeType, factory->mimeTypes()) {
        d->factoriesForMimeType[mimeType].append(factory);
    }

    foreach (const QString &scheme, factory->urlSchemes()) {
        d->factoriesForScheme[scheme].append(factory);
    }

    connect(factory, SIGNAL(destroyed(QObject *)), this, SLOT(onDestroyed1(QObject*)));
}

static void removeFactory(QHash<QString, DocumentManager::FactoryList> &hash, AbstractDocumentFactory *factory)
{
    QMutableHashIterator<QString, DocumentManager::FactoryList> i(hash);
    while (i.hasNext()) {
        i.next();
        DocumentManager::FactoryList &factories = i.value();
        factories.removeAll(factory);
        if (factories.isEmpty())
            i.remove();
    }
}

/*!
  \brief Removes \a factory from DocumentManager.
*/
void DocumentManager::removeFactory(AbstractDocumentFactory *factory)
{
    Q_D(DocumentManager);

    if (!factory)
        return;

    ::removeFactory(d->factoriesForMimeType, factory);
    ::removeFactory(d->factoriesForScheme, factory);

    d->factoriesForId.remove(d->factoriesForId.key(factory));

    disconnect(factory, 0, this, 0);
}

/*!
  \internal
*/
void DocumentManager::onDestroyed1(QObject *o)
{
    removeFactory(static_cast<AbstractDocumentFactory *>(o));
}

/*!
  \internal
*/
QString DocumentManager::getMimeType(const QUrl &url) const
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

