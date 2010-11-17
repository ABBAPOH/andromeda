#include "ifile.h"

IFile::IFile(QObject *parent) :
    QObject(parent)
{
}

/*!
    \fn bool IFile::save(const QString &path)
    \brief Save contents of a file at \a path.
*/

/*!
    \fn bool IFile::isModified()
    \brief Returns true if file was modified.
*/

/*!
    \fn bool IFile::isReadOnly()
    \brief Returns true if file cannot be modified.
*/

/*!
    \fn QString IFile::name()
    \brief Returns display name of this file.
*/

/*!
    \fn QString IFile::path()
    \brief Returns path to file as provided to IEditor::open() function.
*/

/*!
    \fn void IFile::changed()
    \brief This signal is emited every time when file changes.

    For example, when file is being modified, this signal is emited.
*/
