#include "toolmodel.h"

#include <QtCore/QPointer>

#include "abstractdocument.h"

namespace GuiSystem {

class ToolModelPrivate
{
public:
    QPointer<AbstractDocument> m_document;
    QString title;
};

} // namespace GuiSystem

using namespace GuiSystem;

/*!
    \class ToolModel

    ToolModel is a base class for a non-gui part of a ToolWidget.
*/

/*!
    Creates ToolModel with the given parent.
*/
ToolModel::ToolModel(QObject *parent) :
    QObject(parent),
    d_ptr(new ToolModelPrivate)
{
}

ToolModel::~ToolModel()
{
    delete d_ptr;
}

/*!
    Returns current document, or 0 if none was set.
*/
AbstractDocument * ToolModel::document() const
{
    Q_D(const ToolModel);
    return d->m_document;
}

/*!
    Sets current document and emits documentChanged() signal if nececcary.
*/
void ToolModel::setDocument(AbstractDocument *document)
{
    Q_D(ToolModel);
    if (d->m_document == document)
        return;

    d->m_document = document;

    emit documentChanged();
}

/*!
    \fn void ToolModel::documentChanged()

    This signal is emitted when current document is changed.
*/

/*!
    \property ToolModel::title

    \brief Title that is shown to user.

    Title is displayed in a dock widget's header.
*/

QString ToolModel::title() const
{
    Q_D(const ToolModel);
    return d->title;
}

/*!
    \brief Sets current model's title.

    This function should be called by ToolModel implementations when
    they need to update title.
*/
void ToolModel::setTitle(const QString &title)
{
    Q_D(ToolModel);
    if (d->title == title)
        return;

    d->title = title;
    emit titleChanged(title);
}

/*!
    \fn void ToolModel::titleChanged(const QString &icon)

    \brief This signal is emitted when model's title is changed.
*/
