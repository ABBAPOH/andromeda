#ifndef TOOLMODEL_H
#define TOOLMODEL_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

namespace GuiSystem {

class AbstractDocument;

class ToolModelPrivate;
class GUISYSTEM_EXPORT ToolModel : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolModel)
    Q_DISABLE_COPY(ToolModel)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    explicit ToolModel(QObject *parent = 0);
    ~ToolModel();

    AbstractDocument *document() const;
    virtual void setDocument(AbstractDocument *document);

    QString title() const;

signals:
    void documentChanged();
    void titleChanged(const QString &title);

protected:
    void setTitle(const QString &title);

protected:
    ToolModelPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // TOOLMODEL_H
