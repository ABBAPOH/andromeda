#ifndef ABSTRACTEDITORFACTORY_H
#define ABSTRACTEDITORFACTORY_H

#include "coreplugin_global.h"

#include <QtCore/QObject>

class QWidget;

namespace CorePlugin {

class AbstractEditor;

class COREPLUGIN_EXPORT AbstractEditorFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditorFactory)

public:
    explicit AbstractEditorFactory(QObject *parent = 0);
    ~AbstractEditorFactory();

    AbstractEditor *editor(QWidget *parent);

    virtual QStringList mimeTypes() = 0;
    virtual QByteArray id() const = 0;

protected:
    virtual AbstractEditor *createEditor(QWidget *parent) = 0;

private:
    QList<AbstractEditor *> m_editors;

    friend class AbstractEditor;
};

} // namespace CorePlugin

#endif // ABSTRACTEDITORFACTORY_H
