#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>

namespace GuiSystem {

class AbstractEditor;
class AbstractEditorFactory;
class AbstractViewFactory;

class EditorManagerPrivate;
class GUISYSTEM_EXPORT EditorManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorManager)
    Q_DISABLE_COPY(EditorManager)

public:
    explicit EditorManager(QObject *parent = 0);
    ~EditorManager();

    static EditorManager *instance();

    AbstractEditor *editorForId(const QString &id, QWidget *parent = 0);
    AbstractEditor *editorForMimeType(const QString &mimeType, QWidget *parent = 0);
    AbstractEditor *editorForScheme(const QString &scheme, QWidget *parent = 0);
    AbstractEditor *editorForUrl(const QUrl &url, QWidget *parent = 0);

    AbstractEditorFactory *factoryForId(const QString &id) const;
    AbstractEditorFactory *factoryForMimeType(const QString &mimeType) const;
    AbstractEditorFactory *factoryForScheme(const QString &scheme) const;
    AbstractEditorFactory *factoryForUrl(const QUrl &url) const;

    QList<AbstractEditorFactory *> factories() const;
    QList<AbstractEditorFactory *> factoriesForMimeType(const QString &mimeType) const;

    void addFactory(AbstractEditorFactory *factory);
    void removeFactory(AbstractEditorFactory *factory);

    AbstractViewFactory *viewfactory(const QString &id) const;
    void addViewFactory(AbstractViewFactory *factory);
    void removeViewFactory(AbstractViewFactory *factory);

private slots:
    void onDestroyed1(QObject *o);
    void onDestroyed2(QObject *o);

protected:
    QString getMimeType(const QUrl &url) const;

protected:
    EditorManagerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // EDITORMANAGER_H
