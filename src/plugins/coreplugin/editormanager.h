#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QHash>

namespace CorePlugin {

class AbstractEditor;
class AbstractEditorFactory;

class EditorManagerPrivate;
class EditorManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorManager)
    Q_DISABLE_COPY(EditorManager)

public:
    explicit EditorManager(QObject *parent = 0);
    ~EditorManager();

    QList<AbstractEditorFactory *> factories() const;
    AbstractEditorFactory *factory(const QString &mimeType) const;
    AbstractEditorFactory *factoryById(const QString &id) const;
    void addFactory(AbstractEditorFactory *factory);
    void removeFactory(AbstractEditorFactory *factory);

    AbstractEditor *editor(const QString &mimeType, QWidget *parent = 0);
    AbstractEditor *editorById(const QString &id, QWidget *parent = 0);

private slots:
    void onDestroyed(QObject *o);

protected:
    EditorManagerPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // EDITORMANAGER_H
