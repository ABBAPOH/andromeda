#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QHash>

namespace CorePlugin {

class AbstractEditor;
class AbstractEditorFactory;

class EditorManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(EditorManager)

public:
    explicit EditorManager(QObject *parent = 0);

    QList<AbstractEditorFactory *> factories() const;
    AbstractEditorFactory *factory(const QString &mimeType) const;
    void addFactory(AbstractEditorFactory *factory);
    void removeFactory(AbstractEditorFactory *factory);

    AbstractEditor *editor(const QString &mimeType, QWidget *parent = 0);

private slots:
    void onDestroyed(QObject *o);

protected:
    QHash<QString, AbstractEditorFactory *> m_factories;
};

} // namespace CorePlugin

#endif // EDITORMANAGER_H
