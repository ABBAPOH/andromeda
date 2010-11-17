#include "ieditor.h"

using namespace Core;

/*!
    \fn IEditor::IEditor(QObject *parent)
    \brief Constructs IEditor object.
*/
IEditor::IEditor(QObject *parent) :
    QObject(parent)
{
}

/*!
    \fn bool IEditor::open(const QUrl &url)
    \brief Opens file, specified at \a url in this editor.
*/

/*!
    \fn bool IEditor::create()
    \brief Creates new empty file in current editor.
*/

/*!
    \fn IFile *IEditor::file()
    \brief Returns file currenlty opened in editor.

    In fact, editor can have multiple opened files, but this function returns only current one.
*/

/*!
    \fn QWidget *IEditor::widget()
    \brief Returns widget that displays the content of an editor.
*/

/*!
    \fn QToolBar *IEditor::toolBar()
    \brief Returns toolbar with editor actions.
*/

/*!
    \fn QString IEditor::name()
    \brief Returns display name of an editor.
*/

/*!
    \fn void IEditor::changed()
    \brief This signal is emited every time when something in editor changes.

    For example, when you switch file in editor, this signal is emited.
*/

