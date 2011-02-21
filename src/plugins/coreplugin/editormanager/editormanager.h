#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../coreplugin_global.h"

#include <QtCore/QObject>
#include <QtCore/QMetaType>

class QUrl;
namespace CorePlugin
{

class IEditor;
class IEditorFactory;
class EditorManagerPrivate;
class COREPLUGIN_EXPORT EditorManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditorManager)
    Q_DISABLE_COPY(EditorManager)
public:
    explicit EditorManager(QObject *parent = 0);
    ~EditorManager();

    IEditor *openEditor(const QString &path);
    QList<IEditor *>openEditors(const QString &path);

signals:

public slots:

protected:
    EditorManagerPrivate *d_ptr;
};

} // namespace CorePlugin

Q_DECLARE_METATYPE(CorePlugin::EditorManager*)

#endif // EDITORMANAGER_H
