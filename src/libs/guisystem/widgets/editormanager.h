#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>

namespace GuiSystem {

class AbstractEditor;
class AbstractEditorFactory;

class EditorManagerPrivate;
class GUISYSTEM_EXPORT EditorManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorManager)
    Q_DISABLE_COPY(EditorManager)

public:
    typedef QList<AbstractEditorFactory *> FactoryList;

    explicit EditorManager(QObject *parent = 0);
    ~EditorManager();

    static EditorManager *instance();

    AbstractEditorFactory *factoryForId(const QString &id) const;
    AbstractEditorFactory *factoryForMimeType(const QString &mimeType) const;
    AbstractEditorFactory *factoryForScheme(const QString &scheme) const;
    AbstractEditorFactory *factoryForUrl(const QUrl &url) const;

    FactoryList factories() const;
    FactoryList factoriesForMimeType(const QString &mimeType) const;
    FactoryList factoriesForScheme(const QString &scheme) const;
    FactoryList factoriesForUrl(const QUrl &url) const;
    FactoryList factoriesForUrls(const QList<QUrl> &urls) const;

    void addFactory(AbstractEditorFactory *factory);
    void removeFactory(AbstractEditorFactory *factory);

private slots:
    void onDestroyed1(QObject *o);

protected:
    QString getMimeType(const QUrl &url) const;

protected:
    EditorManagerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // EDITORMANAGER_H
