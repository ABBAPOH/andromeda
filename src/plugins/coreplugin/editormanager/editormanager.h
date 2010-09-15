#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../coreplugin_global.h"

#include <QtCore/QObject>

namespace Core
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

    IEditor *openEditor(const QString &file);

signals:

public slots:

protected:
    EditorManagerPrivate *d_ptr;
};

} // namespace Core

#endif // EDITORMANAGER_H
