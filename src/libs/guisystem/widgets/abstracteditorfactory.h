#ifndef ABSTRACTEDITORFACTORY_H
#define ABSTRACTEDITORFACTORY_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

class QWidget;

namespace GuiSystem {

class AbstractEditor;

class GUISYSTEM_EXPORT AbstractEditorFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditorFactory)

public:
    explicit AbstractEditorFactory(const QByteArray &id, QObject *parent = 0);
    ~AbstractEditorFactory();

    AbstractEditor *editor(QWidget *parent);

    QByteArray id() const;

protected slots:
    void onEditorDestroyed(QObject *object);

protected:
    virtual AbstractEditor *createEditor(QWidget *parent) = 0;

    QList<AbstractEditor *> m_editors;
    const QByteArray m_id;

    friend class AbstractEditor;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITORFACTORY_H
