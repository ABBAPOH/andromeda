#include "abstracteditorfactory.h"
#include "abstracteditor.h"

using namespace GuiSystem;

/*!
    \brief Creates a AbstractEditorFactory with the given \a parent.
*/
AbstractEditorFactory::AbstractEditorFactory(QObject *parent) :
    AbstractViewFactory(parent)
{
}

/*!
    \brief Destroys factory and all editors created with it.
*/
AbstractEditorFactory::~AbstractEditorFactory()
{
}

/*!
    \brief Creates new editor and appends it to list of editors.
*/
AbstractEditor * AbstractEditorFactory::editor(QWidget *parent)
{
    AbstractEditor *editor = createEditor(parent);
    editor->setFactory(this);
    m_views.append(editor);
    return editor;
}

AbstractView * AbstractEditorFactory::createView(QWidget *parent)
{
    return createEditor(parent);
}
