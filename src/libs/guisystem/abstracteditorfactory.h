#ifndef ABSTRACTEDITORFACTORY_H
#define ABSTRACTEDITORFACTORY_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QIcon>

class QWidget;

namespace GuiSystem {

class AbstractDocument;
class AbstractEditor;

class GUISYSTEM_EXPORT AbstractEditorFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractEditorFactory)

public:
    explicit AbstractEditorFactory(QObject *parent = 0);
    ~AbstractEditorFactory();

    AbstractDocument *document(QObject *parent);
    AbstractEditor *editor(QWidget *parent);

    virtual QByteArray id() const = 0;
    virtual QString name() const = 0;
    virtual QIcon icon() const = 0;
    virtual QStringList mimeTypes() const { return QStringList(); }
    virtual QStringList urlSchemes() const { return QStringList(); }
    virtual int weight() const { return 50; }

protected slots:
    void onDocumentDestroyed(QObject *object);
    void onEditorDestroyed(QObject *object);

protected:
    virtual AbstractDocument *createDocument(QObject *parent) = 0;
    virtual AbstractEditor *createEditor(QWidget *parent) = 0;

    QList<AbstractDocument *> m_documents;
    QList<AbstractEditor *> m_editors;

    friend class AbstractEditor;
};

} // namespace GuiSystem

#endif // ABSTRACTEDITORFACTORY_H
