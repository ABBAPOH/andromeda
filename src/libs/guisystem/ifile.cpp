#include "ifile.h"

using namespace GuiSystem;

/*!
  \class GuiSystem::IFile

  \brief IFile is an interface for file saving API.
*/

/*!
  \brief Constructs an IFile with the given \a parent.
*/
IFile::IFile(QObject *parent) :
    QObject(parent)
{
}

/*!
  \property IFile::modified

  \brief This property holds whether the document has been modified by the user.
*/

/*!
  \brief Reimplement this function to return if opened document is modified and can be saved.

  Default implementation returns false.
*/
bool IFile::isModified() const
{
    return false;
}

/*!
  \brief Reimplement this function to set if opened document is modified and can be saved.

  Default implementation does nothing.
*/
void IFile::setModified(bool modified)
{
    Q_UNUSED(modified);
}

/*!
  \fn void IFile::modificationChanged(bool modified)

  \brief Emit this signal whenever document changes it's modification state (i.e. modified by user).
*/

/*!
  \property IFile::readOnly

  \brief This property holds whether the document is read only
  (i.e. can be saved with same url) or not.
*/

/*!
  \brief Reimplement this function to return if opened document is readOnly.

  Default implementation returns false.
*/
bool IFile::isReadOnly() const
{
    return true;
}

/*!
  \brief Reimplement this function to set if opened document is read only or not.

  Default implementation does nothing.
*/
void IFile::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
}

/*!
  \brief Reimplement to save currently opened document to \a url.

  Empty url is passed when user requests to save document to current url.
*/
void IFile::save(const QUrl &url)
{
}

/*!
  \fn void IFile::readOnlyChanged(bool readOnly)

  \brief Emit this signal whenever document changes it's modification state (i.e. modified by user).
*/
