#ifndef ABSTRACTEDITORFACTORY_H
#define ABSTRACTEDITORFACTORY_H

#include "guisystem_global.h"

#include "abstractviewfactory.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QWidget;

namespace GuiSystem {

class AbstractEditor;

class GUISYSTEM_EXPORT AbstractEditorFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditorFactory)

public:
    explicit AbstractEditorFactory(QObject *parent = 0);
    ~AbstractEditorFactory();

    AbstractEditor *editor(QWidget *parent);

    virtual QByteArray id() const = 0;
    virtual QStringList mimeTypes() const { return QStringList(); }
    virtual QStringList urlSchemes() const { return QStringList(); }

protected:
    virtual AbstractEditor *createEditor(QWidget *parent) = 0;

    QList<AbstractEditor *> m_editors;

    friend class AbstractEditor;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITORFACTORY_H
