#include "toolwidget.h"

#include "abstracteditor.h"
#include "toolmodel.h"

using namespace GuiSystem;

/*!
    \class ToolWidget

    ToolWidget is a base class for tools (usually, represented by dock widgets)
    in GuiSystem.

    Each tool should be separated into two parts - model (see ToolModel) and a
    view.
*/

/*!
    Creates ToolWidget with the given \a model \a parent.

    \note It is not possible to change model after ToolWidget has been created.
*/
ToolWidget::ToolWidget(ToolModel &model, QWidget *parent) :
    QWidget(parent),
    m_model(&model)
{
}

/*!
    Returns model, associated with this widget.
*/
ToolModel * ToolWidget::model() const
{
    return m_model;
}

/*!
    Returns current editor.
*/
AbstractEditor * ToolWidget::editor() const
{
    return m_editor;
}

/*!
    Sets current editor and emits editorChanged() signal if necessary.

    This method is called by EditorWindow for all ToolWidgets when it's
    current editor changed.
*/
void ToolWidget::setEditor(AbstractEditor *editor)
{
    if (m_editor == editor)
        return;

    if (m_editor)
        disconnect(m_editor, SIGNAL(documentChanged()), this, SLOT(onDocumentChanged()));

    m_editor = editor;

    if (m_editor)
        connect(m_editor, SIGNAL(documentChanged()), this, SLOT(onDocumentChanged()));
    onDocumentChanged();

    emit editorChanged();
}

/*!
    \fn void ToolWidget::editorChanged()

    This signal is emitted when current editor is changed.
*/

/*!
    This method is called each time current editor's document is changed.
*/
void ToolWidget::onDocumentChanged()
{
    AbstractDocument *doc = m_editor ? m_editor->document() : 0;
    m_model->setDocument(doc);
}
