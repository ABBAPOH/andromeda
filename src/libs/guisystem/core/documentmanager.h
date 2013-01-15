#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>

#include "../guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>

namespace GuiSystem {

class AbstractEditor;
class AbstractDocumentFactory;

class DocumentManagerPrivate;
class GUISYSTEM_EXPORT DocumentManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DocumentManager)
    Q_DISABLE_COPY(DocumentManager)

public:
    typedef QList<AbstractDocumentFactory *> FactoryList;

    explicit DocumentManager(QObject *parent = 0);
    ~DocumentManager();

    static DocumentManager *instance();

    AbstractDocumentFactory *factoryForId(const QString &id) const;
    AbstractDocumentFactory *factoryForMimeType(const QString &mimeType) const;
    AbstractDocumentFactory *factoryForScheme(const QString &scheme) const;
    AbstractDocumentFactory *factoryForUrl(const QUrl &url) const;

    FactoryList factories() const;
    FactoryList factoriesForMimeType(const QString &mimeType) const;
    FactoryList factoriesForScheme(const QString &scheme) const;
    FactoryList factoriesForUrl(const QUrl &url) const;
    FactoryList factoriesForUrls(const QList<QUrl> &urls) const;

    void addFactory(AbstractDocumentFactory *factory);
    void removeFactory(AbstractDocumentFactory *factory);

private slots:
    void onDestroyed1(QObject *o);

protected:
    QString getMimeType(const QUrl &url) const;

protected:
    DocumentManagerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // DOCUMENTMANAGER_H
